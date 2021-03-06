/**
* @Author   Guillaume Labey
*/

#include <Engine/Core/Components/ScriptComponent.hh>
#include <Engine/Sound/SoundManager.hpp>
#include <Engine/Systems/RenderingSystem.hpp>
#include <Engine/Systems/ScriptSystem.hpp>
#include <Engine/Systems/UISystem.hpp>
#include <Engine/Utils/Helper.hpp>
#include <Engine/Window/GameWindow.hpp>
#include <Game/Character/Enemy.hpp>
#include <Game/Character/NewBuild.hpp>
#include <Game/GameStates/PlayState.hpp>
#include <Game/Manager/GameManager.hpp>
#include <Game/Manager/GoldManager.hpp>
#include <Game/Scripts/CheatCodesScript.hpp>

#include <Game/GameStates/ConsoleState.hpp>

Entity::sHandle ConsoleState::_lastBuiltBaseTurretHandle = 0;

ConsoleState::~ConsoleState() {}

void    ConsoleState::onEnter()
{
    SoundManager::getInstance()->setVolumeAllChannels(0.25f);
}

void    ConsoleState::setupSystems()
{
    _world.addSystem<ScriptSystem>();
    _world.addSystem<UISystem>();
    _world.addSystem<RenderingSystem>(nullptr);
}

bool    ConsoleState::init()
{
    auto em = _world.getEntityManager();
    _console = em->getEntityByTag("Console");

    if (!_console)
    {
        LOG_ERROR("ConsoleState::init: Can't find entity with tag \"Console\"");
    }

    uint32_t stateNb = static_cast<uint32_t>(_gameStateManager->getStates().size());
    if (stateNb == 1)
    {
        LOG_ERROR("ConsoleState::handleCheatCodeKillAll: ConsoleState has to be on PlayState level");
        return (false);
    }

    return (true);
}

bool    ConsoleState::update(float elapsedTime)
{
    auto &&keyboard = GameWindow::getInstance()->getKeyboard();

    // Display the previous states
    renderPreviousStates({PlayState::identifier});

    bool success = GameState::update(elapsedTime);

    // Unpause the game
    if (keyboard.getStateMap()[Keyboard::eKey::ESCAPE] == Keyboard::eKeyState::KEY_PRESSED)
    {
        return (false);
    }
    else if (keyboard.getStateMap()[Keyboard::eKey::ENTER] == Keyboard::eKeyState::KEY_PRESSED)
    {
        if (!_console)
        {
            LOG_ERROR("ConsoleState::update: Can't enter cheat codes because the entity is not found");
            return (false);
        }

        CheatCodesScript* cheatCodeScripts = sScriptComponent::getEntityScript<CheatCodesScript>(_console, "CheatCodesScript");
        if (!cheatCodeScripts)
        {
            LOG_ERROR("ConsoleState::update: Can't enter cheat codes because the console entity doesn not have the script \"CheatCodesScript\"");
            return (false);
        }

        // Handle cheat code
        {
            uint32_t stateNb = static_cast<uint32_t>(_gameStateManager->getStates().size());
            auto playState = _gameStateManager->getStates()[stateNb - 2];
            EntityManager* playStateEntityManager = playState->getWorld().getEntityManager();

            EntityFactory::bindEntityManager(playStateEntityManager);
            handleCheatCode(static_cast<PlayState*>(playState.get()), cheatCodeScripts->getText().getContent());
            EntityFactory::bindEntityManager(_world.getEntityManager());
        }

        return (false);
    }

    return (success);
}

void    ConsoleState::handleCheatCodeKillAll(PlayState* playState)
{
    EntityManager* playStateEntityManager = playState->getWorld().getEntityManager();
    const auto& enemies = playStateEntityManager->getEntitiesByTag("Enemy");

    for (auto& enemy : enemies)
    {
        Enemy*  enemyScript = sScriptComponent::getEntityScript<Enemy>(enemy, "Enemy");

        if (!enemyScript)
        {
            LOG_ERROR("ConsoleState::handleCheatCodeKillAll: Can't find script \"Enemy\" on enemy");
            continue;
        }

        enemyScript->death();
    }
}

void    ConsoleState::handleCheatCodeGiveMeGold(PlayState* playState)
{
    EntityManager* playStateEntityManager = playState->getWorld().getEntityManager();
    Entity* gameManager = playStateEntityManager->getEntityByTag(GAME_MANAGER_TAG);

    if (!gameManager)
    {
        LOG_ERROR("ConsoleState::handleCheatCodeGiveMeGold: Can't find entity with tag \"%s\"", GAME_MANAGER_TAG);
        return;
    }

    GoldManager* goldManager = sScriptComponent::getEntityScript<GoldManager>(gameManager, GOLD_MANAGER_TAG);

    if (!goldManager)
    {
        LOG_ERROR("ConsoleState::handleCheatCodeGiveMeGold: Can't find script \"%s\" on gameManager", GOLD_MANAGER_TAG);
        return;
    }

    goldManager->addGolds(777);
}

void    ConsoleState::handleCheatCodeBuildForMe(PlayState* playState)
{
    EntityManager* playStateEntityManager = playState->getWorld().getEntityManager();
    NewBuild* newBuild = nullptr;
    GameManager* gameManager = nullptr;

    // Get NewBuild script
    {
        Entity* player = playStateEntityManager->getEntityByTag("Player");

        if (!player)
        {
            LOG_ERROR("ConsoleState::handleCheatCodeBuildForMe: Can't find entity with tag \"Player\"");
            return;
        }

        newBuild = sScriptComponent::getEntityScript<NewBuild>(player, "NewBuild");

        if (!newBuild)
        {
            LOG_ERROR("ConsoleState::handleCheatCodeBuildForMe: Can't find script \"NewBuild\" on player");
            return;
        }
    }

    // Get GameManager script
    {
        Entity* gameManagerEntity = playStateEntityManager->getEntityByTag(GAME_MANAGER_TAG);

        if (!gameManagerEntity)
        {
            LOG_ERROR("ConsoleState::handleCheatCodeBuildForMe: Can't find entity with tag \"%s\"", GAME_MANAGER_TAG);
            return;
        }

        gameManager = sScriptComponent::getEntityScript<GameManager>(gameManagerEntity, GAME_MANAGER_TAG);

        if (!gameManager)
        {
            LOG_ERROR("ConsoleState::handleCheatCodeBuildForMe: Can't find script \"%s\" on gameManager", GAME_MANAGER_TAG);
            return;
        }
    }

    bool canBuild = true;
    while (canBuild && !gameManager->map.isFull())
    {
        Entity* tile = nullptr;
        bool found = false;
        glm::ivec2 pos;
        while (!found)
        {
            pos.x = Helper::randInt(0, gameManager->map.getWidth() - 1);
            pos.y = Helper::randInt(0, gameManager->map.getHeight() - 1);

            if (gameManager->map.isWalkable(pos.x, pos.y))
            {
                found = true;
                tile = gameManager->map.getEntities()[pos.x][pos.y];
            }
        }

        Entity* lastBuiltBaseTurret = playStateEntityManager->getEntity(_lastBuiltBaseTurretHandle);

        // A base turret has been built previously
        // But the tower could not be built (Out of money)
        if (!lastBuiltBaseTurret)
        {
            // Build base turret
            newBuild->setCurrentChoice("TILE_BASE_TURRET");
            newBuild->setTileHovered(tile);
            // Build and get the created entity
            lastBuiltBaseTurret = newBuild->placePreviewedEntity();
            newBuild->setCurrentChoice("");
        }

        // Build tower if base turret has been built
        if (lastBuiltBaseTurret)
        {
            newBuild->setCurrentChoice("TOWER_FIRE");
            newBuild->setTileHovered(lastBuiltBaseTurret);
            canBuild = newBuild->placePreviewedEntity() != nullptr;
            newBuild->setCurrentChoice("");

            if (!canBuild)
            {
                _lastBuiltBaseTurretHandle = lastBuiltBaseTurret->handle;
            }
        }
        else
        {
            canBuild = false;
        }

        if (canBuild)
        {
            _lastBuiltBaseTurretHandle = 0;
        }
    }
}

void    ConsoleState::handleCheatCodePlayForMe(PlayState* playState)
{
    playState->_autoPlay = true;
}

void    ConsoleState::handleCheatCodeAegis(PlayState* playState)
{
    // time speed is a member of GameState class
    // It speeds up the delta time send to GameState::_systems update
    if (playState->getTimeSpeed() == 1.0f)
    {
        playState->setTimeSpeed(4.0f);
    }
    else
    {
        playState->setTimeSpeed(1.0f);
    }
}

void    ConsoleState::handleCheatCodeWin(PlayState* playState)
{
    playState->_autoWin = true;
}

void    ConsoleState::handleCheatCodeLose(PlayState* playState)
{
    playState->_autoLose = true;
}

void    ConsoleState::handleCheatCode(PlayState* playState, const std::string& cheatCode)
{
    if (cheatCode == "killall")
    {
        handleCheatCodeKillAll(playState);
    }
    else if (cheatCode == "croesus")
    {
        handleCheatCodeGiveMeGold(playState);
    }
    else if (cheatCode == "autobuild")
    {
        handleCheatCodeBuildForMe(playState);
    }
    else if (cheatCode == "autoplay")
    {
        handleCheatCodePlayForMe(playState);
    }
    else if (cheatCode == "aegis")
    {
        handleCheatCodeAegis(playState);
    }
    else if (cheatCode == "victory")
    {
        handleCheatCodeWin(playState);
    }
    else if (cheatCode == "defeat")
    {
        handleCheatCodeLose(playState);
    }
}
