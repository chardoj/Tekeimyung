/**
* @Author   Guillaume Labey
*/

#include <Engine/Core/GameStateManager.hpp>

GameStateManager::GameStateManager() {}

GameStateManager::~GameStateManager() {}

void    GameStateManager::removeCurrentState()
{
    if (hasStates())
    {
        _states.pop_back();
        if (hasStates())
        {
            _states.back()->bindEntityManager();
            _states.back()->onEnter();
        }
    }
}

void    GameStateManager::removeBackState()
{
    if (hasStates())
    {
        _states.pop_back();
    }
}

std::shared_ptr<GameState> GameStateManager::getCurrentState() const
{
    if (hasStates())
        return (_states.back());
    return (nullptr);
}

bool    GameStateManager::hasStates() const
{
    return (_states.size() != 0);
}

void    GameStateManager::clearStates()
{
    _states.clear();
}
