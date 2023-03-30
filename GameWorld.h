#ifndef GAMEWORLD_H_
#define GAMEWORLD_H_

#include "GameConstants.h"
#include <map>
#include <string>
#include <queue>
#include <chrono>

class GameController;

class GameWorld
{
public:

	GameWorld(std::string assetPath)
	 : m_stars(0), m_coins(0), m_boardNumber(1), m_controller(nullptr),
	   m_assetPath(assetPath)
	{
		m_keyMap = {
			{ 'a',             { 1, ACTION_LEFT } },
			{ 'd',             { 1, ACTION_RIGHT } },
			{ 'w',             { 1, ACTION_UP } },
			{ 's',             { 1, ACTION_DOWN } },
			{ KEY_PRESS_TAB,   { 1, ACTION_ROLL } },
			{ '`',             { 1, ACTION_FIRE } },
			{ KEY_PRESS_LEFT,  { 2, ACTION_LEFT } },
			{ KEY_PRESS_RIGHT, { 2, ACTION_RIGHT } },
			{ KEY_PRESS_UP,    { 2, ACTION_UP } },
			{ KEY_PRESS_DOWN,  { 2, ACTION_DOWN } },
			{ KEY_PRESS_ENTER, { 2, ACTION_ROLL } },
			{ '\\',            { 2, ACTION_FIRE } },
		};

		if (!m_assetPath.empty()  &&  m_assetPath.back() != '/')
			m_assetPath.push_back('/');
	}

	virtual ~GameWorld()
	{
	}

	virtual int init() = 0;
	virtual int move() = 0;
	virtual void cleanUp() = 0;

	void setGameStatText(std::string text);

	int getAction(int playerNum);
	void playSound(int soundID);

	int getBoardNumber() const
	{
		return m_boardNumber;
	}

	void setFinalScore(int stars, int coins)
	{
		m_stars = stars;
		m_coins = coins;
	}

	void startCountdownTimer(int numSeconds)
	{
		m_countdownTimerDeadline = std::chrono::system_clock::now() +
			std::chrono::seconds(numSeconds);
	}

	int timeRemaining() const
	{
		auto dur = m_countdownTimerDeadline - std::chrono::system_clock::now();
		return static_cast<int>(dur.count() / decltype(dur)::period::den);
	}

	std::string assetPath() const
	{
		return m_assetPath;
	}

	  // The following should be used by only the framework, not the student

	void setBoardNumber(int boardNumber)
	{
		m_boardNumber = boardNumber;
	}
 
	void setController(GameController* controller)
	{
		m_controller = controller;
	}

	int getWinnerStars() const
	{
		return m_stars;
	}

	int getWinnerCoins() const
	{
		return m_coins;
	}

	void setMsPerTick(int ms_per_tick);

private:
	struct KeyInfo
	{
		int playerNum;
		int action;
	};

	int             m_lives;
	int             m_stars;
	int             m_coins;
	int             m_boardNumber;
	GameController* m_controller;
	std::queue<int> m_pendingActions[2];  // 0 for Peach, 1 for Yoshi
	std::string     m_assetPath;
	std::map<int, KeyInfo> m_keyMap;
	std::chrono::system_clock::time_point m_countdownTimerDeadline;
};

#endif // GAMEWORLD_H_
