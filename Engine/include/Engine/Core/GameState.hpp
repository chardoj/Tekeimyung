/**
* @Author   Guillaume Labey
*/

#pragma once

#include <memory>
#include <vector>

#include <ECS/World.hpp>
#include <ECS/EntityManager.hpp>

class GameWindow;
class GameStateManager;

class GameState
{
friend class GameStateManager;
friend class GameWindow;

public:
    GameState(GameStateManager* gameStateManager, uint32_t id, const std::string& levelFile = "");
    virtual ~GameState();

    virtual bool            init() = 0;
    virtual void            onEnter() {}

    virtual void            setupSystems() = 0;
    virtual bool            initSystems();
    virtual bool            update(float elapsedTime);

    void                    bindEntityManager();

    uint32_t                getId() const;
    float                   getTimeSpeed() const;
    World&                  getWorld();

    void                    setLevelFile(const std::string& levelFile);
    void                    setTimeSpeed(float timeSpeed);

    void                    cloneEntityManager(EntityManager* em);

protected:
    void                    renderPreviousStates(const std::vector<uint32_t>& filterIds = {});

private:
    void                    loadLevel();
    void                    onWindowResize();

protected:
    World                   _world;

    GameStateManager*       _gameStateManager;
    std::string             _levelFile;

    uint32_t                _id;

    float                   _timeSpeed = 1.0f;
};


struct GameStateFactory
{
    virtual std::shared_ptr<GameState> create(GameStateManager* gameStateManager) = 0;
    virtual std::string getLevelFile() = 0;
};

template<typename GameStateType>
class BaseGameState: public GameState
{
    public:
        BaseGameState(GameStateManager* gameStateManager, const std::string& levelFile = ""): GameState(gameStateManager, GameStateType::identifier, levelFile) {}
};

template <typename... Args>
static std::string  getString(Args... args)
{
    return std::string(args...);
}

#define START_GAMESTATE(name, ...)                                                                      \
    class name : public BaseGameState<name> {                                                           \
        public:                                                                                         \
            name(GameStateManager* gameStateManager): BaseGameState(gameStateManager, ## __VA_ARGS__) {}   \
            static constexpr unsigned int identifier = #name##_crc32;                                   \
            static std::string getLevelFile()                                                           \
            {                                                                                           \
                return (getString(__VA_ARGS__));                                                        \
            }

#define END_GAMESTATE(name)                                                                     \
    };                                                                                          \
    struct name##Factory: public GameStateFactory                                               \
    {                                                                                           \
        std::shared_ptr<GameState> create(GameStateManager* gameStateManager) override final    \
        {                                                                                       \
            return std::make_shared<name>(gameStateManager);                                    \
        }                                                                                       \
                                                                                                \
        std::string getLevelFile() override final                                               \
        {                                                                                       \
            return (name::getLevelFile());                                                      \
        }                                                                                       \
        static constexpr unsigned int identifier = name::identifier;                            \
    };
