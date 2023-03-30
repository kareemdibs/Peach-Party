// CS32 Project 3
// Kareem Dibs

#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, int dir, int depth, double size) :GraphObject(imageID, startX, startY, dir, depth, size), ifAlive(true) {
    m_world = world;
}

// Returns a pointer to the game world that the actor belongs to
StudentWorld* Actor::getWorld() const {
    return m_world;
}

// Returns whether the actor is currently alive
bool Actor::isAlive() const {
    return ifAlive;
}

// Sets the actor to be dead
void Actor::setDead() {
    ifAlive = false;
}

// Returns whether the actor is living (i.e. can move or take actions), which is false by default
bool Actor::isLiving() {
    return false;
}

// Returns whether the actor can be impacted by other objects in the game, which is false by default
bool Actor::isImpactable() const {
    return false;
}

// Empty method to be overridden by subclasses for specific hit behaviors
void Actor::getHit() {}

MovingActor::MovingActor(StudentWorld* world, int imageID, int startX, int startY) :
    Actor(world, imageID, startX, startY, right, 0, 1), walkDir(right), state(PAUSED_OR_WAITING), m_ticks_to_move(0) {}

bool MovingActor::isLiving() {
    return true;
}

int MovingActor::RandomDirectionMovement() {
    int newDirection;
    // Keep randomly generating new directions until a valid one is found
    do {
        newDirection = randInt(0, 3) * 90;
    } while (!canMove(newDirection));
    return newDirection;
}

void MovingActor::teleport() {
    int x, y;
    // Keep generating random coordinates until an empty spot is found
    do {
        x = randInt(0, 15);
        y = randInt(0, 15);
    } while (getWorld()->isEmpty(x, y));
    moveTo(x * 16, y * 16);
}

//void MovingActor::teleport() {
//    int x, y;
//    bool foundEmptySpot = false;
//    // Continue generating random coordinates until an empty spot is found
//    while (!foundEmptySpot) {
//        x = randInt(0, 15);
//        y = randInt(0, 15);
//        if (!getWorld()->isEmpty(x, y)) {
//            foundEmptySpot = true;
//            moveTo(x * 16, y * 16);
//        }
//    }
//}

bool MovingActor::canMove(int direction) {
    int newX, newY;
    getPositionInThisDirection(direction, 16, newX, newY);
    newX /= 16;
    newY /= 16;
    return !getWorld()->isEmpty(newX, newY);
}

void MovingActor::cornerMove() {
    // Determine which direction to turn based on current walk direction
    if (getWalkDir() == right || getWalkDir() == left) {
        if (canMove(up)) {
            setWalkDir(up);
        }
        else {
            setWalkDir(down);
        }
    }
    else if (getWalkDir() == up || getWalkDir() == down) {
        if (canMove(right)) {
            setWalkDir(right);
        }
        else {
            setWalkDir(left);
        }
    }
}

bool MovingActor::isFork() {
    int validDirections = 0; //Number of valid directions
    if (canMove(up)) {
        validDirections++;
    }
    if (canMove(right)) {
        validDirections++;
    }
    if (canMove(down)) {
        validDirections++;
    }
    if (canMove(left)) {
        validDirections++;
    }
    if (validDirections >= 3) {
        return true;
    }
    return false;
}

void MovingActor::move() {
    moveAtAngle(walkDir, 2); //Move 2 pixels in the walk direction
    setTicksToMove(getTicksToMove() - 1); //Decrement ticks to move count by 1
    if (getTicksToMove() == 0) {
        moveFunction();
    }
}

int MovingActor::getWalkDir() const {
    return walkDir;
}

int MovingActor::getState() const {
    return state;
}

int MovingActor::getTicksToMove() const {
    return m_ticks_to_move;
}

void MovingActor::setState(int new_state) {
    state = new_state;
}

// Set the walking direction of the MovingActor to the given direction newDir
void MovingActor::setWalkDir(int newDir) {
    // Update the walking direction member variable to the new direction
    walkDir = newDir;

    // Adjust the sprite direction based on the new walking direction.
    // If the new walking direction is left, set the sprite direction to left (180 deg).
    // Otherwise, if the new walking direction is not left, set the sprite direction to right (0 deg).
    if (newDir == left) {
        setDirection(left);
    }
    else {
        setDirection(right);
    }
}

void MovingActor::setTicksToMove(int ticks) {
    m_ticks_to_move = ticks;
}

Player::Player(StudentWorld* world, int imageID, int startX, int startY, int pNum)
    :MovingActor(world, imageID, startX, startY), m_dieRoll(0), m_coins(0), m_stars(0), vortex(false), landed(false), on_square(false), on_directional_square(false), teleported(false), m_playerNum(pNum) {}

//This function performs actions for the player character
void Player::doSomething() {
    //If the player is waiting for their turn
    if (getState() == WAITING) {
        //Check if the player has been teleported and is facing an invalid direction, and turn to a random valid direction if so
        if (teleported && !canMove(getWalkDir())) {
            int newDirection = RandomDirectionMovement();
            setWalkDir(newDirection);
            setTeleportedFlag(false);
        }
        //Get player's chosen action from the game world
        int playerAction = getWorld()->getAction(m_playerNum);
        //If the player chooses to roll the die
        if (playerAction == ACTION_ROLL) {
            //Generate a random die roll and update the ticks to move accordingly
            m_dieRoll = randInt(1, 10);
            setTicksToMove(m_dieRoll * 8);
            setState(WALKING); //Change player state to walking
            landed = false; //Player moves away from square
            on_square = false; //Resets flag for square interaction
        }
        //If the player chooses to fire a vortex
        else if (hasVortex() && playerAction == ACTION_FIRE) {
            //Create a vortex and play the sound effect
            getWorld()->createVortex(getX(), getY(), getWalkDir());
            //Update player to no longer have vortex
            adjustVortex();
        }
        else return; //User doesn't press key or presses another key
    }
    //If the player is walking
    if (getState() == WALKING) {
        //Only check if on a square
        if (getX() % 16 == 0 && getY() % 16 == 0) {
            //Check for directional square and if true change direction done in directional square itself
            if (on_directional_square) {
                setOnDirectionalSquare(false);
            }
            //If player is at a fork
            else if (isFork()) {
                //Check if the user selects a direction from the game world
                int newDirection = getWorld()->getAction(m_playerNum);
                if (newDirection == ACTION_UP) {
                    newDirection = up;
                }
                else if (newDirection == ACTION_RIGHT) {
                    newDirection = right;
                }
                else if (newDirection == ACTION_DOWN) {
                    newDirection = down;
                }
                else if (newDirection == ACTION_LEFT) {
                    newDirection = left;
                }
                else {
                    newDirection = -1;
                }
                //If the new direction is valid, update the walk direction and sprite direction accordingly
                if (newDirection != -1 && canMove(newDirection)) {
                    setWalkDir(newDirection);
                }
                else return;
            }//Else if at a fork
            //If the player can't move in their current direction, adjust their walk direction accordingly
            else if (!canMove(getWalkDir())) {
                cornerMove();
            }
        }
        move(); //Move the player character
    }
}


// This function is called when the player has finished walking for the allotted number of ticks.
// It sets the player's state back to WAITING to roll and updates the landed flag to true to indicate that the player has landed on a square.
void Player::moveFunction() {
    setState(WAITING); //If ticks to move is equal to 0, then change the state to waiting to roll
    landed = true; //Player has landed on square
}

//Getters

int Player::isWaiting() const {
    return getState() == WAITING;
}

bool Player::hasLanded() const {
    return landed;
}

bool Player::isStillOnSquare() const {
    return on_square;
}

int Player::getStars() const {
    return m_stars;
}

int Player::getCoins() const {
    return m_coins;
}

bool Player::hasVortex() const {
    return vortex;
}

int Player::getRoll() const {
    return m_dieRoll;
}

//Setters

void Player::setHere(bool indicator) {
    on_square = indicator;
}

void Player::addStars(int stars) {
    m_stars += stars;
}

void Player::addCoins(int coins) {
    m_coins += coins;
}

void Player::adjustVortex() {
    vortex = !vortex;
}

void Player::setRoll(int roll) {
    m_dieRoll = roll;
}

void Player::setOnDirectionalSquare(bool indicator) {
    on_directional_square = indicator;
}

void Player::setTeleportedFlag(bool indicator) {
    teleported = indicator;
}

Baddie::Baddie(StudentWorld* world, int imageID, int startX, int startY) : MovingActor(world, imageID, startX, startY), travelDist(0), pauseCounter(180), squaresToMove(0), peach(false), yoshi(false) {}

// This function is called by the Baddie object to execute its actions.
void Baddie::doSomething() {
    // If the Baddie is paused
    if (getState() == PAUSED) {
        // Check if the Baddie is on the same square as Peach and Peach is in a waiting state
        if (getX() == getWorld()->getPeach()->getX() && getY() == getWorld()->getPeach()->getY() && getWorld()->getPeach()->isWaiting()) {
            // Check if Peach is a new player
            if (!inContactWithPeach()) {
                // Execute the Baddie's action with Peach
                actionWhenPaused(getWorld()->getPeach());
                // Mark Peach as having interacted with Baddie already
                setPeach(true);
            } // if Peach is new player
        } // if Baddie is on the same square as Peach
        else {
            // Peach moves away
            setPeach(false);
        }

        // Check if the Baddie is on the same square as Yoshi and Yoshi is in a waiting state
        if (getX() == getWorld()->getYoshi()->getX() && getY() == getWorld()->getYoshi()->getY() && getWorld()->getYoshi()->isWaiting()) {
            // Check if Yoshi is a new player
            if (!inContactWithYoshi()) {
                // Execute the Baddie's action with Yoshi
                actionWhenPaused(getWorld()->getYoshi());
                // Mark Yoshi as having interacted with Baddie already
                setYoshi(true);
            } // if Yoshi is new player
        } // if Baddie is on the same square as Yoshi
        else {
            // Yoshi moves away
            setYoshi(false);
        }

        // Decrement the pause counter
        pauseCounter--;
        if (pauseCounter == 0) {
            // Set squares to move to a random number between 1 and 10
            squaresToMove = randnumSquares();
            setTicksToMove(squaresToMove * 8);

            // Pick a new random direction to walk in, that is legal
            int newDirection = RandomDirectionMovement();
            setWalkDir(newDirection);

            // Set the Baddie to walking state
            setState(WALKING);
            // Reset the interaction status of Peach and Yoshi
            setPeach(false);
            setYoshi(false);
        }
    } // if Baddie is in the paused state
    // If the Baddie is walking
    if (getState() == WALKING) {
        // If the Baddie is on top of a square
        if (getX() % 16 == 0 && getY() % 16 == 0 && isFork()) {
            // Pick a random direction to walk in, that is legal
            int newDirection = RandomDirectionMovement();
            setWalkDir(newDirection);
        }
        // Else if the Baddie cannot move forward
        else if (getX() % 16 == 0 && getY() % 16 == 0 && !canMove(getWalkDir())) {
            // Move to the nearest corner
            cornerMove();
        }
        // Move the Baddie
        move();
    }
}

bool Baddie::inContactWithPeach() const {
    return peach;
}

bool Baddie::inContactWithYoshi() const {
    return yoshi;
}

bool Baddie::isImpactable() const {
    return true;
}

void Baddie::setPeach(bool indicator) {
    peach = indicator;
}

void Baddie::setYoshi(bool indicator) {
    yoshi = indicator;
}

void Baddie::setPauseCounter(int counter) {
    pauseCounter = counter;
}

void Baddie::getHit() {
    //Teleport baddie to a random square on the board
    teleport();
    //Set baddie's walking direction to the right and sprite direction to 0 degrees
    setWalkDir(right);
    //Transition baddie's state to paused
    setState(PAUSED);
    //Set the counter for how many ticks the baddie should remain paused to 180.
    setPauseCounter(180);
}

Bowser::Bowser(StudentWorld* world, int imageID, int startX, int startY) : Baddie(world, imageID, startX, startY) {}

void Bowser::actionWhenPaused(Player* m_player) {
    //Flip a coin to determine if Bowser will activate
    bool activateBowser = (rand() % 2 == 0); // 50% chance
    if (activateBowser) {
        //Take away all coins and stars from the player
        m_player->addCoins(-m_player->getCoins());
        m_player->addStars(-m_player->getStars());
        //Play a sound effect when Bowser activates
        getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
    }//if Bowser causes player to lose all coins/stars
}

//void Bowser::actionWhenPaused(Player* m_player) {
//    //Simulate 50% chance
//    int coinFlip = randInt(1, 2);
//    if (coinFlip == 1) {
//        //Cause player to lose all their stars and coins
//        m_player->addCoins(-m_player->getCoins());
//        m_player->addStars(-m_player->getStars());
//        getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
//    }//if bowser causes player to lose all coins/stars
//}

//This function sets the state of Bowser to paused and sets the pause counter to 180
//This function is called when Bowser is to move to another square on the board
void Bowser::moveFunction() {
    //Set Bowser to paused state
    setState(PAUSED);
    //Set pause ticks to 180
    setPauseCounter(180);
    int probability = randInt(1, 4); // 25% chance
    if (probability == 4) {
        getWorld()->convertSquareToDropping(getX() / 16, getY() / 16);
    }
}

int Bowser::randnumSquares() {
    return randInt(1, 10); // return random integer from 1 to 10
}

Boo::Boo(StudentWorld* world, int imageID, int startX, int startY) : Baddie(world, imageID, startX, startY) {}

void Boo::actionWhenPaused(Player* m_player) {
    //Get pointers to Peach and Yoshi
    Player* p1 = getWorld()->getPeach();
    Player* p2 = getWorld()->getYoshi();

    int coinFlip = randInt(1, 2); // 50% chance
    //Swap coins or stars between players depending on coin flip result
    if (coinFlip == 1) {
        //Swap coins
        //Store p1's coins in a temporary variable
        int tempC = p1->getCoins();

        //Swap p1's coins with p2's coins
        p1->addCoins(-tempC + p2->getCoins());

        //Swap p1's coins with p2's coins
        p2->addCoins(-p2->getCoins() + tempC);
    }
    else {
        //Swap stars
        //Store p1's stars in a temporary variable
        int tempS = p1->getStars();

        //Swap p1's stars with p2's stars
        p1->addStars(-tempS + p2->getStars());

        //Swap p2's stars with the value in the temporary variable
        p2->addStars(-p2->getStars() + tempS);
    }
}

//This function sets the state of Boo to paused and sets the pause counter to 180
//This function is called when Boo is to move to another square on the board
void Boo::moveFunction() {
    //Set Boo to paused state
    setState(PAUSED);
    //Set pause ticks to 180
    setPauseCounter(180);
}

Square::Square(StudentWorld* world, int imageID, int startX, int startY) :Actor(world, imageID, startX, startY, right, 1, 1) {}


/*This function checks if the square is alive. If it is not, the function returns.
The function then checks if Peach or Yoshi have traversed the square. If one of them has,
the function checks if they have landed on the square. If they have, the function checks if
they are new to the square or not. If they are new, it marks them as having activated the square
and calls the landingOnSquare function. If they are not new, the function calls the actionMoving function.*/
void Square::doSomething() {
    // Check if the square is still alive
    if (!isAlive()) {
        return;
    }
    // Check if Peach has traversed the square
    if (getX() == getWorld()->getPeach()->getX() && getY() == getWorld()->getPeach()->getY()) {
        // Check if Peach has landed on the square
        if (getWorld()->getPeach()->hasLanded()) {
            // Check if Peach is new to the square
            if (!getWorld()->getPeach()->isStillOnSquare()) {
                // Mark Peach as having activated the square
                getWorld()->getPeach()->setHere(true);
                // Call the landingOnSquare function for Peach
                landingOnSquare(getWorld()->getPeach());
            }
        }
        else {
            // If Peach has not landed on the square, call the actionMoving function
            actionMoving(getWorld()->getPeach());
        }
    }

    // Check if Yoshi has traversed the square
    if (getX() == getWorld()->getYoshi()->getX() && getY() == getWorld()->getYoshi()->getY()) {
        // Check if Yoshi has landed on the square
        if (getWorld()->getYoshi()->hasLanded()) {
            // Check if Yoshi is new to the square
            if (!getWorld()->getYoshi()->isStillOnSquare()) {
                // Mark Yoshi as having activated the square
                getWorld()->getYoshi()->setHere(true);
                // Call the landingOnSquare function for Yoshi
                landingOnSquare(getWorld()->getYoshi());
            }
        }
        else {
            // If Yoshi has not landed on the square, call the actionMoving function
            actionMoving(getWorld()->getYoshi());
        }
    }
}

int Boo::randnumSquares() {
    return randInt(1, 3); // return random integer from 1 to 3
}

CoinSquare::CoinSquare(StudentWorld* world, int imageID, int startX, int startY, int amt) :Square(world, imageID, startX, startY), coinAdjuster(amt) {}

void CoinSquare::landingOnSquare(Player* m_player) {
    //If blue coin square, add coins to player
    if (coinAdjuster > 0) {
        m_player->addCoins(coinAdjuster);
        //Play sound effect for adding coins
        getWorld()->playSound(SOUND_GIVE_COIN);
    }
    else {
        //If red coin square, deduct coins from player and ensure coins don't go below 0
        int playerCoins = m_player->getCoins();
        if (playerCoins < 3) {
            m_player->addCoins(-playerCoins);
        }
        else {
            m_player->addCoins(coinAdjuster);
        }
        //Play sound effect for deducting coins
        getWorld()->playSound(SOUND_TAKE_COIN);
    }
}

void CoinSquare::actionMoving(Player* m_player) 
{}

StarSquare::StarSquare(StudentWorld* world, int imageID, int startX, int startY) :Square(world, imageID, startX, startY) {}

void StarSquare::landingOnSquare(Player* m_player) {
    // Check if player has enough coins to purchase a star
    if (m_player->getCoins() < 20) {
        // If not, return without doing anything
        return;
    }
    else {
        // Otherwise, deduct 20 coins from player's total
        m_player->addCoins(-20);
        // Give the player a star
        m_player->addStars(1);
        // Play sound for giving a star
        getWorld()->playSound(SOUND_GIVE_STAR);
    }
}

void StarSquare::actionMoving(Player* m_player)
{ 
    landingOnSquare(m_player); 
}

DirectionalSquare::DirectionalSquare(StudentWorld* world, int imageID, int startX, int startY, int fDir) :Square(world, imageID, startX, startY), forcingDir(fDir) {
    setDirection(fDir);
}

void DirectionalSquare::landingOnSquare(Player* m_player) {
    // Change the player's walking direction and inform them that they have landed on a directional square.
    m_player->setWalkDir(forcingDir);
    m_player->setOnDirectionalSquare(true);
}

void DirectionalSquare::actionMoving(Player* m_player)
{ 
    landingOnSquare(m_player); 
}

BankSquare::BankSquare(StudentWorld* world, int imageID, int startX, int startY) :Square(world, imageID, startX, startY) {}

void BankSquare::landingOnSquare(Player* m_player) {
    //Add coins to player's balance
    int bankAmount = getWorld()->getBankBalance();
    m_player->addCoins(bankAmount);
    //Reset central bank
    getWorld()->setBankBalance(0);
}

void BankSquare::actionMoving(Player* m_player) {
    int playerBalance = m_player->getCoins();
    if (playerBalance < 5) {
        //Player does not have enough coins to deposit
        //Set player's coins to 0 and add to central bank
        m_player->addCoins(-playerBalance);
        getWorld()->setBankBalance(getWorld()->getBankBalance() + playerBalance);
        //Play deposit sound effect
        getWorld()->playSound(SOUND_DEPOSIT_BANK);
    }
    else {
        //Player has enough coins to deposit
        //Deduct 5 coins from player and add to central bank
        m_player->addCoins(-5);
        getWorld()->setBankBalance(getWorld()->getBankBalance() + 5);
        //Play withdraw sound effect
        getWorld()->playSound(SOUND_WITHDRAW_BANK);
    }
}

EventSquare::EventSquare(StudentWorld* world, int imageID, int startX, int startY) :Square(world, imageID, startX, startY) {}

void EventSquare::landingOnSquare(Player* m_player) {
    // Choose a random event
    int randomEvent = randInt(1, 3);
    if (randomEvent == 1) {
        // Teleport player to a random square
        // Teleport the player to a random valid square
        m_player->teleport();

        // Play teleport sound effect
        getWorld()->playSound(SOUND_PLAYER_TELEPORT);

        // Set the teleported flag to true for the player
        m_player->setTeleportedFlag(true);
    }
    else if (randomEvent == 2) {
        // Swap players
        // Get pointers to player1 and player2
        Player* player1 = getWorld()->getPeach();
        Player* player2 = getWorld()->getYoshi();

        // Store player1's values in temporary variables
        int tempX = player1->getX();
        int tempY = player1->getY();
        int tempDieRoll = player1->getRoll();
        int tempState = player1->getState();
        int tempWalkDir = player1->getWalkDir();
        int tempTicksToMove = player1->getTicksToMove();

        // Swap player1's values with player2's values
        player1->moveTo(player2->getX(), player2->getY());
        player1->setRoll(player2->getRoll());
        player1->setState(player2->getState());
        player1->setWalkDir(player2->getWalkDir());
        player1->setTicksToMove(player2->getTicksToMove());

        // Swap player2's values with the temporary variables
        player2->moveTo(tempX, tempY);
        player2->setRoll(tempDieRoll);
        player2->setState(tempState);
        player2->setWalkDir(tempWalkDir);
        player2->setTicksToMove(tempTicksToMove);

        // Update the "here" flags for both players
        if (m_player == player1) {
            player1->setHere(false);
            player2->setHere(true);
        }
        else {
            player1->setHere(true);
            player2->setHere(false);
        }

        // Play teleport sound effect
        getWorld()->playSound(SOUND_PLAYER_TELEPORT);
    }
    else if (randomEvent == 3) {
        // Give player1 a vortex
        // If player1 doesn't already have a vortex, give them one and play sound effect
        if (!m_player->hasVortex()) {
            m_player->adjustVortex();
            getWorld()->playSound(SOUND_GIVE_VORTEX);
        }
    }
}

void EventSquare::actionMoving(Player* m_player) 
{}

DroppingSquare::DroppingSquare(StudentWorld* world, int imageID, int startX, int startY) :Square(world, imageID, startX, startY) {}

void DroppingSquare::landingOnSquare(Player* m_player) {
    // Randomly choose an option between two possibilities
    int randomOption = randInt(1, 2);

    // If option 1 is chosen, deduct 10 coins, or as many as possible if the player has less than 10 coins, from the player
    if (randomOption == 1) {
        int val = m_player->getCoins();
        if (val < 10) {
            m_player->addCoins(-val);
        }
        else {
            m_player->addCoins(-10);
        }
    }
    // If option 2 is chosen, deduct a star from the player if they have at least one star
    else {
        if (m_player->getStars() > 0) {
            m_player->addStars(-1);
        }
    }

    // Play the sound effect for landing on a dropping square
    getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
}

void DroppingSquare::actionMoving(Player* m_player) 
{}

Vortex::Vortex(StudentWorld* world, int imageID, int startX, int startY, int dir) :
    Actor(world, imageID, startX, startY, right, 0, 1), dir_walking(dir) {}

void Vortex::doSomething() {
    //If the vortex is not alive, exit the function
    if (!isAlive()) { 
        return; 
    }
    //Move the vortex 2 pixels in its walking direction
    moveAtAngle(dir_walking, 2);

    //If the vortex goes out of bounds, mark it as dead and exit the function
    if (getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT) {
        setDead();
        return;
    }

    //Check if there's an actor at the vortex's location that can be impacted
    Actor* impactedActor = getWorld()->checkImpactOfVortex(getX(), getY());

    //If there is an actor at the vortex's location
    if (impactedActor != nullptr) {
        //The impacted actor is hit by the vortex
        impactedActor->getHit();

        //Mark the vortex as dead
        setDead();

        //Play the hit by vortex sound
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
}