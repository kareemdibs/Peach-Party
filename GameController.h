#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

#include "SpriteManager.h"
#include <string>
#include <map>
#include <queue>
#include <iostream>
#include <sstream>

class GraphObject;
class GameWorld;

class GameController
{
  public:
	void run(int argc, char* argv[], GameWorld* gw, std::string windowTitle);

	bool getKeyIfAny(int& key)
	{
		if (m_keysHit.empty())
			return false;
		key = m_keysHit.front();
		m_keysHit.pop_front();
		return true;
	}

	void putBackKey(int key)
	{
		m_keysHit.push_front(key);
	}

	void playSound(int soundID);

	void setGameStatText(std::string text)
	{
		m_gameStatText = text;
	}

	void doSomething();

	void reshape(int w, int h);
	void keyboardEvent(unsigned char key, int x, int y);
	void specialKeyboardEvent(int key, int x, int y);

	  // Meyers singleton pattern
	static GameController& getInstance()
	{
		static GameController instance;
		return instance;
	}

	static void timerFuncCallback(int nothing);
	void setMsPerTick(int ms_per_tick) { m_ms_per_tick = ms_per_tick;  }

private:
	enum GameControllerState : int;

	struct KeyMapInfo
	{
		int key;
		int action;
	};

	GameWorld*	m_gw;
	GameControllerState m_gameState;
	GameControllerState m_nextStateAfterPrompt;
	GameControllerState m_nextStateAfterAnimate;
	std::deque<int> m_keysHit;
	bool        m_singleStep;
    bool        m_postInitPreCleanup;
	std::string m_gameStatText;
	std::string m_mainMessage;
	std::string m_secondMessage;
	int	        m_curIntraFrameTick;
	int         m_winner;
	std::map<int, std::string> m_soundMap;
	std::map<int, std::string> m_imageNameMap;
	std::map<int, KeyMapInfo> m_keyMap;
	SpriteManager m_spriteManager;

	void setGameState(GameControllerState s);
	void initDrawersAndSounds();
	bool passesThruWhenSingleStepping(int key) const;
	void displayGamePlay();
	void reportLeakedGraphObjects() const;

	static const int kDefaultMsPerTick = 10;
	static int m_ms_per_tick;
};

inline GameController& Game()
{
	return GameController::getInstance();
}

#endif // GAMECONTROLLER_H_
