/**
* @Author   Guillaume Labey
*/

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <iostream>

#include <Engine/Core/Engine.hpp>
#include <Engine/EditorState.hpp>
#include <Engine/EntityFactory.hpp>
#include <Engine/Utils/Exception.hpp>
#include <Engine/Utils/EventSound.hpp>
#include <Engine/Utils/LevelLoader.hpp>
#include <Engine/Utils/Logger.hpp>
#include <Engine/Utils/RessourceManager.hpp>
#include <Engine/Utils/Debug.hpp>

#include <Game/GameStates/ConfirmExitState.hpp>
#include <Game/GameStates/HowToPlayState.hpp>
#include <Game/GameStates/OptionsMenuState.hpp>
#include <Game/GameStates/PauseState.hpp>
#include <Game/GameStates/PlayState.hpp>

void    windowCloseHandler(void* data)
{
    Engine* engine = static_cast<Engine*>(data);
    auto& gameStateManager = engine->getGameStateManager();

    // Only add ConfirmExitState if not already in the states
    if (gameStateManager.getCurrentState() &&
        gameStateManager.getCurrentState()->getId() != ConfirmExitState::identifier)
    {
        gameStateManager.addState<ConfirmExitState>();
    }
}

int     main()
{
    Engine engine;
    auto &&gameStateManager = engine.getGameStateManager();
    try
    {
        if (!engine.init())
            return (1);

        // Load entities after engine initialization to have logs
        EntityFactory::loadDirectory(ARCHETYPES_LOCATION);
        // Load textures, models & sounds
        RessourceManager::getInstance()->loadResources("resources");

        // Load levels
        LevelLoader::getInstance()->loadDirectory(LEVELS_DIRECTORY);
        REGISTER_GAMESTATE(ConfirmExitState);
        REGISTER_GAMESTATE(HowToPlayState);
        REGISTER_GAMESTATE(OptionsMenuState);
        REGISTER_GAMESTATE(PauseState);
        REGISTER_GAMESTATE(PlayState);

        EventSound::loadEvents();
        GameWindow::getInstance()->registerCloseHandler(windowCloseHandler, &engine);

        std::shared_ptr<GameState> state = nullptr;
        #if defined(ENGINE_DEBUG) && ENGINE_DEBUG == true
            state = std::make_shared<EditorState>(&gameStateManager);
        #else
            state = std::make_shared<PlayState>(&gameStateManager);
        #endif

        if (!gameStateManager.addState(state))
            return (1);
        else if (!engine.run())
            return (1);
    }
    catch(const Exception& e)
    {
        LOG_ERROR("ERROR: %s", e.what());
    }

    engine.stop();
    return (0);
}
