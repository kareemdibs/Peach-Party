#include "GameWorld.h"
#include "GameController.h"
#include <string>
using namespace std;

int GameWorld::getAction(int playerNum)
{
    queue<int>& pendingActions = m_pendingActions[playerNum-1];
    if (!pendingActions.empty())
    {
        int action = pendingActions.front();
        pendingActions.pop();
        return action;
    }
    int key;
    while (m_controller->getKeyIfAny(key))
    {
        auto it = m_keyMap.find(key);
        if (it == m_keyMap.end())  // meaningless key
            continue;
        const KeyInfo& keyInfo = it->second;
        if (keyInfo.playerNum == playerNum)
            return keyInfo.action;
        m_pendingActions[keyInfo.playerNum-1].push(keyInfo.action);
    }
    return ACTION_NONE;
}

void GameWorld::playSound(int soundID)
{
	m_controller->playSound(soundID);
}

void GameWorld::setGameStatText(string text)
{
	m_controller->setGameStatText(text);
}

void GameWorld::setMsPerTick(int ms_per_tick)
{
	m_controller->setMsPerTick(ms_per_tick);
}
