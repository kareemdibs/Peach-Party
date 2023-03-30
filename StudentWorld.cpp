// CS32 Project 3
// Kareem Dibs

#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::~StudentWorld() {
    //Prevents double delete of actors
    if (actors.size() > 0) {
        cleanUp();
    }
}

StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath), centralBankAccount(0)
{
}

int StudentWorld::init()
{
    //Load board
    string board_file = assetPath() + "board0" + to_string(getBoardNumber()) + ".txt";
    Board::LoadResult result = m_board.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found || result == Board::load_fail_bad_format) return GWSTATUS_BOARD_ERROR; //Returns GWSTATUS_BOARD_ERROR is board is improperly formatted or missing

    //Dynammically Allocate Actors
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int q = 0; q < BOARD_HEIGHT; q++) {
            switch (m_board.getContentsOf(q, i)) {
                //Empty tile
            case(Board::empty):
                break;
                //Player tile
            case(Board::player):
                Peach = new Player(this, IID_PEACH, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, 1);
                Yoshi = new Player(this, IID_YOSHI, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, 2);
                actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, 3));
                break;
                //Blue coin tile
            case(Board::blue_coin_square):
                actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, 3));
                break;
                //Red coin tile
            case(Board::red_coin_square):
                actors.push_back(new CoinSquare(this, IID_RED_COIN_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, -3));
                break;
                //Star square tile
            case(Board::star_square):
                actors.push_back(new StarSquare(this, IID_STAR_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT));
                break;
                //Up directional square tile
            case(Board::up_dir_square):
                actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, Actor::up));
                break;
                //Right direcitonal square tile
            case(Board::right_dir_square):
                actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, Actor::right));
                break;
                //Down directional square tile
            case(Board::down_dir_square):
                actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, Actor::down));
                break;
                //Left directional square tile
            case(Board::left_dir_square):
                actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, Actor::left));
                break;
                //Bank square tile
            case(Board::bank_square):
                actors.push_back(new BankSquare(this, IID_BANK_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT));
                break;
                //Event square tile
            case(Board::event_square):
                actors.push_back(new EventSquare(this, IID_EVENT_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT));
                break;
                //Boswer tile
            case(Board::bowser):
                actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, 3));
                actors.push_back(new Bowser(this, IID_BOWSER, q * SPRITE_WIDTH, i * SPRITE_HEIGHT));
                break;
                //Boo tile
            case(Board::boo):
                actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, q * SPRITE_WIDTH, i * SPRITE_HEIGHT, 3));
                actors.push_back(new Boo(this, IID_BOO, q * SPRITE_WIDTH, i * SPRITE_HEIGHT));
                break;
            }
        }
    }
    startCountdownTimer(99);  // Start game timer of 99 seconds
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //Ask actors to do something
    Peach->doSomething();
    Yoshi->doSomething();
    // Ask all other actors to do something
    for (int i = 0; i < actors.size(); i++) {
        actors[i]->doSomething();
    }

    // Delete any dead actors
    for (vector<Actor*>::iterator p = actors.begin(); p < actors.end();) {
        if (!((*p)->isAlive())) {
            Actor* deletedSoon = (*p);
            p = actors.erase(p);
            delete deletedSoon;
        }
        else {
            p++;
        }
    }

    // Update game stats with current player stats, time remaining, and bank balance
    string p1Stats = "P1 Roll: " + to_string(Peach->getRoll()) + " Stars: " + to_string(Peach->getStars()) + " $$: " + to_string(Peach->getCoins());
    Peach->hasVortex() ? p1Stats += " VOR | " : p1Stats += " | ";

    string p2Stats = "P2 Roll: " + to_string(Yoshi->getRoll()) + " Stars: " + to_string(Yoshi->getStars()) + " $$: " + to_string(Yoshi->getCoins());
    if (Yoshi->hasVortex()) p2Stats += " VOR";

    setGameStatText(p1Stats + "Time: " + to_string(timeRemaining()) + " | Bank: " + to_string(getBankBalance()) + " | " + p2Stats);

    // End the game if time is up
    if (timeRemaining() <= 0) {
        // Play sound effect to signal end of game
        playSound(SOUND_GAME_FINISHED);

        // Get the final number of stars and coins for each player
        int pStars = Peach->getStars();
        int yStars = Yoshi->getStars();
        int pCoins = Peach->getCoins();
        int yCoins = Yoshi->getCoins();

        // Determine the winner of the game based on stars and coins
        if (pStars == yStars) {
            // If the number of stars is tied, determine the winner based on coins
            if (pCoins == yCoins) {
                int winner = randInt(1, 2); // random winner if stars and coins are equal
                setFinalScore(pStars, pCoins);
                if (winner == 1) {
                    setFinalScore(pStars, pCoins);
                    return GWSTATUS_PEACH_WON;
                }
                else {
                    setFinalScore(yStars, yCoins);
                    return GWSTATUS_YOSHI_WON;
                }
            }
            else if (pCoins > yCoins) {
                // If Peach has more coins than Yoshi, Peach wins
                setFinalScore(pStars, pCoins);
                return GWSTATUS_PEACH_WON;
            }
            else {
                // If Yoshi has more coins than Peach, Yoshi wins
                setFinalScore(yStars, yCoins);
                return GWSTATUS_YOSHI_WON;
            }
        }
        else if (pStars > yStars) {
            // If Peach has more stars than Yoshi, Peach wins
            setFinalScore(pStars, pCoins);
            return GWSTATUS_PEACH_WON;
        }
        else {
            // If Yoshi has more stars than Peach, Yoshi wins
            setFinalScore(yStars, yCoins);
            return GWSTATUS_YOSHI_WON;
        }
    }

    // If the game is not over yet, return the continue game status
    return GWSTATUS_CONTINUE_GAME;
}

// This function is responsible for cleaning up the game by deleting all actors
// and the Peach and Yoshi players before the game exits.
void StudentWorld::cleanUp()
{
    //Delete Peach and Yoshi players
    delete Peach;
    delete Yoshi;
    // Loop through the vector of actors and delete each actor until the vector is empty
    while (actors.size() > 0) {
        Actor* toBeDeleted = actors[0];
        actors.erase(actors.begin());
        delete toBeDeleted;
    }
}

int StudentWorld::getBankBalance() const {
    return centralBankAccount;
}

void StudentWorld::setBankBalance(int amt) {
    centralBankAccount = amt;
}

Player* StudentWorld::getPeach() {
    return Peach;
}

Player* StudentWorld::getYoshi() {
    return Yoshi;
}

//Checks if a tile on the board is empty
bool StudentWorld::isEmpty(int x, int y) {
    if (m_board.getContentsOf(x, y) == Board::empty) { 
        return true; 
    }
    return false;
}

void StudentWorld::convertSquareToDropping(int x, int y) {
    // Look through all actors to find the square below Bowser
    for (int j = 0; j < actors.size(); j++) {
        // If the actor is a dead square and is directly below Bowser
        if (!actors[j]->isLiving() && actors[j]->getX() / 16 == x && actors[j]->getY() / 16 == y) {
            // Set the dead square to be deleted
            actors[j]->setDead();
            break;
        }
    }
    // Create a new dropping square in the place of the deleted square
    actors.push_back(new DroppingSquare(this, IID_DROPPING_SQUARE, x * SPRITE_WIDTH, y * SPRITE_HEIGHT));
    // Play sound to indicate that a dropping square has been created
    playSound(SOUND_DROPPING_SQUARE_CREATED);
}

//This function checks if an impactable actor has come into contact with a vortex
//It takes in the x and y coordinates of the vortex to check for overlap
//Returns the impactable actor that is overlapping with the vortex, if there is one
//Otherwise returns nullptr
Actor* StudentWorld::checkImpactOfVortex(int x, int y) {
    Actor* actor = nullptr;
    //Iterate through all actors in the game
    for (int k = 0; k < actors.size(); k++) {
        //If actor is impactable
        if (actors[k]->isImpactable()) {
            //Check if the actor overlaps with the vortex
            if (abs(actors[k]->getX() - x) < SPRITE_WIDTH && abs(actors[k]->getY() - y) < SPRITE_HEIGHT) {
                //If there is overlap, set the actor to be returned and break out of the loop
                actor = actors[k];
                break;
            }
        }
    }
    return actor;
}

void StudentWorld::createVortex(int x, int y, int dir) {
    // Create a new vortex actor with the given parameters
    actors.push_back(new Vortex(this, IID_VORTEX, x, y, dir));
    // Play the sound for player firing a vortex
    playSound(SOUND_PLAYER_FIRE);
}