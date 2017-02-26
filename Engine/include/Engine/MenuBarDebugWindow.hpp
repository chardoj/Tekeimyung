/**
* @Author   Guillaume Labey
*/

#pragma once

#include <ECS/EntityManager.hpp>
#include <Engine/Utils/DebugWindow.hpp>
#include <Engine/Core/GameStateManager.hpp>

class Engine;

class MenuBarDebugWindow: public DebugWindow
{
friend Engine;

public:
    MenuBarDebugWindow(GameStateManager* gameStateManager, EntityManager* em, const glm::vec2& pos, const glm::vec2& size);
    virtual ~MenuBarDebugWindow();

    void                build(std::shared_ptr<GameState> gameState, float elapsedTime) override final;

    void                loadLevel(const std::string& levelName);

    GENERATE_ID(MenuBarDebugWindow);

private:
    void                displayLevelsMenu();
    void                displayPlayStopMenu();

    void                displaySaveAsPopup();
    void                displayLoadPopup();

    void                play();

private:
    std::string         _currentLevel;
    std::string         _tmpLoadLevel;

    GameStateManager*   _gameStateManager;
    // Entity manager of EditorState
    EntityManager*      _em;
};