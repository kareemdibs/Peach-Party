// CS32 Project 3
// Kareem Dibs

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "Board.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    // Constructor takes in a path to the assets directory
    StudentWorld(std::string assetPath);

    // Destructor cleans up any dynamically allocated memory
    ~StudentWorld();

    // Initializes the game world and returns an integer indicating success or failure
    virtual int init();

    // Runs a single game tick and returns an integer indicating success or failure
    virtual int move();

    // Cleans up the game world by deallocating dynamically allocated memory
    virtual void cleanUp();

    // Returns true if the tile at the given coordinates is empty, false otherwise
    bool isEmpty(int x, int y);

    // Returns a pointer to the Peach player character
    Player* getPeach();

    // Returns a pointer to the Yoshi player character
    Player* getYoshi();

    // Returns the number of coins in the central bank
    int getBankBalance() const;

    // Sets the number of coins in the central bank to the given amount
    void setBankBalance(int amt);

    // Converts the tile at the given coordinates to a dropping tile
    void convertSquareToDropping(int x, int y);

    // Checks if the vortex at the given coordinates makes an impact with an impactable actor,
    // and if so, returns a pointer to the first such actor encountered in the actors vector
    Actor* checkImpactOfVortex(int x, int y);

    // Creates a vortex at the given coordinates and in the given direction, and plays a sound
    void createVortex(int x, int y, int dir);
private:
    // A vector containing all actors in the game world except for Peach and Yoshi
    std::vector<Actor*> actors;

    // Pointers to the Peach and Yoshi player characters
    Player* Peach;
    Player* Yoshi;

    // The game board
    Board m_board;

    // The number of coins in the central bank
    int centralBankAccount;
};

#endif // STUDENTWORLD_H_