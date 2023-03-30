// CS32 Project 3
// Kareem Dibs

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject {
public:
    // Constructor
    // Creates an actor with the specified image, position, direction, depth, and size
    Actor(StudentWorld* world, int imageID, int startX, int startY, int dir = right, int depth = 0, double size = 1.0);

    // Pure virtual function to be implemented by derived classes
    // Represents the action the actor takes during a single game tick
    virtual void doSomething() = 0;

    // Virtual function to indicate whether the actor is a living object
    // Only applies to player and enemy objects
    virtual bool isLiving();

    // Function to check if the actor is currently alive or dead
    bool isAlive() const;

    // Function to mark the actor as dead
    void setDead();

    // Virtual function to indicate whether the actor can be impacted by other objects in the game
    virtual bool isImpactable() const;

    // Virtual function to specify what happens to the actor when it is impacted by a vortex object
    virtual void getHit();

    // Accessor function to retrieve the world the actor belongs to
    StudentWorld* getWorld() const;

private:
    // Sprite direction of the actor
    int spriteDirection;

    // Indicates whether the actor is currently alive or dead
    bool ifAlive;

    // Pointer to the world the actor belongs to
    StudentWorld* m_world;
};

//Base class for player and baddie
class MovingActor : public Actor {
public:
    // Constructor
    // Creates a moving actor with the specified image and position
    MovingActor(StudentWorld* world, int imageID, int startX, int startY);

    // Pure virtual function to be implemented by derived classes
    // Represents the action the actor takes during a single game tick
    virtual void doSomething() = 0;

    // Virtual function to indicate whether the actor is a living object
    // Only applies to player and enemy objects
    bool isLiving();

    // Helper function to check if the actor can move in the specified direction
    bool canMove(int direction);

    // Helper function to check if the actor is at a corner and can't move forward
    // If so, choose an appropriate direction to turn
    void cornerMove();

    // Helper function to check if there is a fork in the actor's path
    bool isFork();

    // Helper function to generate a new valid random direction for the actor
    int RandomDirectionMovement();

    // Function to teleport the actor to a random location on the game screen
    void teleport();

    // Virtual function to move the actor
    virtual void move();

    // Pure virtual function to be implemented by derived classes
    // Represents the action the actor takes when the number of ticks to move is zero
    virtual void moveFunction() = 0;

    // Getters to retrieve information about the actor
    int getWalkDir() const; //Gets player/baddie walk direction
    int getState() const; //Gets state
    int getTicksToMove() const; //Gets number of ticks player/baddie has left to move

    // Setters to modify the state of the actor
    void setWalkDir(int newDir); //Sets walk direction
    void setState(int new_state); //Sets player/baddie state
    void setTicksToMove(int ticks); //Sets the number of ticks player has left to move

private:
    // Constants
    static const int PAUSED_OR_WAITING = 0; // Default state for player/baddie

    // Member variables
    int m_ticks_to_move; // Number of ticks the actor has left to move
    int walkDir; // Walk direction of the actor
    int state; // Walking or waiting state of the actor
};

// Player class for Peach and Yoshi
class Player : public MovingActor {
public:
    // Constructor
    Player(StudentWorld* world, int imageID, int startX, int startY, int playerNum);

    // Overridden functions
    void doSomething();
    void moveFunction();

    // Getters
    
    // Returns player's die roll
    int getRoll() const;
    // Returns whether player is in waiting state
    int isWaiting() const;
    // Returns whether player has landed on a square
    bool hasLanded() const;
    // Returns whether player is still on square
    bool isStillOnSquare() const; 
    // Returns the number of stars player has
    int getStars() const; 
    // Returns the number of coins player has
    int getCoins() const; 
    // Returns whether player has vortex
    bool hasVortex() const; 

    // Setters

    // Sets here flag to indicator
    void setHere(bool indicator);
    // Adds coins to player's coins
    void addCoins(int coins);
    // Adds stars to player's stars
    void addStars(int stars);
    // Adjusts vortex flag
    void adjustVortex();
    // Sets player's roll to roll
    void setRoll(int roll);
    // Sets flag to indicator if player is on a Directional Square
    void setOnDirectionalSquare(bool indicator);
    // Sets teleported flag to indicator
    void setTeleportedFlag(bool indicator);

private:
    // Constants
    static const int WAITING = 0;
    static const int WALKING = 1;

    // Member variables
    int m_playerNum; // Player number
    int m_dieRoll; // Player's die roll
    int m_coins; // Number of coins
    int m_stars; // Number of stars
    bool landed; // Flag for when the player lands on a square
    bool on_square; // Flag for if a player is staying on a square
    bool on_directional_square; // Flag for when player lands on directional square
    bool teleported; // Flag for when player is teleported
    bool vortex; // Whether player has a vortex projectile or not
    
};

//Baddie base class for Bowser and Boo
class Baddie : public MovingActor {
public:
    Baddie(StudentWorld* world, int imageID, int startX, int startY);
    // Overridden function that controls what baddie does each tick
    void doSomething();

    // Pure virtual function that controls what baddie does when they are paused
    virtual void actionWhenPaused(Player* m_player) = 0;

    // Pure virtual function that gets a random number of squares to move
    virtual int randnumSquares() = 0;

    // Pure virtual function that controls what baddie does when they are walking
    virtual void moveFunction() = 0;

    // Function that is called when baddie is hit by a vortex
    void getHit();

    // Getter function that returns whether baddie is in contact with Peach
    bool inContactWithPeach() const;

    // Getter function that returns whether baddie is in contact with Yoshi
    bool inContactWithYoshi() const;

    // Getter function that returns whether baddie can be impacted by projectiles
    bool isImpactable() const;

    // Setter function that sets Peach contact flag to tf
    void setPeach(bool indicator);

    // Setter function that sets Yoshi contact flag to tf
    void setYoshi(bool indicator);

    // Setter function that sets pause counter to amt
    void setPauseCounter(int counter);

private:
    // Constants for baddie states
    static const int PAUSED = 0;
    static const int WALKING = 1;
    int travelDist; // Distance to travel
    int squaresToMove; // Squares to move
    int pauseCounter; // Counter for pause state
    bool peach; // Flag for Peach contact
    bool yoshi; // Flag for Yoshi contact

};

//Derived class for bowser
class Bowser : public Baddie {
public:
    // Constructor takes in a pointer to the StudentWorld, image ID, starting x and y coordinates
    Bowser(StudentWorld* world, int imageID, int startX, int startY);
    // Overridden function for what Bowser does when paused, takes in a pointer to the player
    void actionWhenPaused(Player* plyr);

    // Function to return random number of squares to move
    int randnumSquares();

    // Specialized function for Bowser movement
    void moveFunction();
private:
    // Constants for Bowser's states
    static const int PAUSED = 0;
    static const int WALKING = 1;
};

//Derived class for boo
class Boo : public Baddie {
public:
    // Constructor takes in a pointer to the StudentWorld, image ID, starting x and y coordinates
    Boo(StudentWorld* world, int imageID, int startX, int startY);
    // Function to be called when boo is paused
    void actionWhenPaused(Player* m_player);

    // Function to return random number of squares for boo to move
    int randnumSquares();

    // Function for specialized boo movement
    void moveFunction();
private:
    // Constants for Boo's states
    static const int PAUSED = 0;
    static const int WALKING = 1;
};

// This is the base class for all the types of squares in a game. It derives from Actor class.
class Square : public Actor {
public:
    // Constructor for creating a square
    Square(StudentWorld* world, int imageID, int startX, int startY); 
    // Virtual function to be overridden by derived classes
    virtual void doSomething(); 
    // Pure virtual function for performing square action when player lands on it
    virtual void landingOnSquare(Player* m_player) = 0;
    // Pure virtual function for performing square action when player traverses it
    virtual void actionMoving(Player* m_player) = 0;
private:
};

// This derived class represents a coin square.
class CoinSquare : public Square {
public:
    // Constructor for creating a coin square
    CoinSquare(StudentWorld* world, int imageID, int startX, int startY, int amt); 
    // Overrides the landingOnSquare function for coin square action
    void landingOnSquare(Player* m_player);
    // Overrides the actionMoving function for coin square action
    void actionMoving(Player* m_player);
private:
    int coinAdjuster; // The number of coins to give/take
};

// This derived class represents a star square.
class StarSquare : public Square {
public:
    // Constructor for creating a star square
    StarSquare(StudentWorld* world, int imageID, int startX, int startY);
    // Overrides the landingOnSquare function for star square action
    void landingOnSquare(Player* m_player);
    // Overrides the actionMoving function for star square action
    void actionMoving(Player* m_player);
private:
};

// This derived class represents a directional square.
class DirectionalSquare : public Square {
public:
    // Constructor for creating a directional square
    DirectionalSquare(StudentWorld* world, int imageID, int startX, int startY, int fDir);
    // Overrides the landingOnSquare function for directional square action
    void landingOnSquare(Player* m_player);
    // Overrides the actionMoving function for directional square action
    void actionMoving(Player* m_player);
private:
    int forcingDir; // The direction in which it forces the player to move
};

// This derived class represents a bank square.
class BankSquare : public Square {
public:
    // Constructor for creating a bank square
    BankSquare(StudentWorld* world, int imageID, int startX, int startY);
    // Overrides the landingOnSquare function for bank square action
    void landingOnSquare(Player* m_player);
    // Overrides the actionMoving function for bank square action
    void actionMoving(Player* m_player);
private:
};

// This derived class represents an event square.
class EventSquare : public Square {
public:
    // Constructor for creating an event square
    EventSquare(StudentWorld* world, int imageID, int startX, int startY);
    // Overrides the landingOnSquare function for event square action
    void landingOnSquare(Player* m_player);
    // Overrides the actionMoving function for event square action
    void actionMoving(Player* m_player);
private:
};

// This derived class represents a dropping square.
class DroppingSquare : public Square {
public:
    // Constructor for creating a dropping square
    DroppingSquare(StudentWorld* world, int imageID, int startX, int startY);
    // Overrides the landingOnSquare function for dropping square action
    void landingOnSquare(Player* m_player);
    // Overrides the actionMoving function for dropping square action
    void actionMoving(Player* m_player);
private:
};

// Derived class for vortex
class Vortex : public Actor {
public:
    // Constructor for creating a new vortex with the given starting coordinates and direction
    Vortex(StudentWorld* world, int imageID, int startX, int startY, int dir);
    // The doSomething function is called once per game tick, and updates the state of the vortex
    void doSomething();
private:
    int dir_walking;
};

#endif // ACTOR_H_