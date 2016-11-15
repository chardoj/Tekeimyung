#include <Game/Components.hh>

#include <Game/Systems/WaveSystem.hpp>

WaveSystem::WaveSystem(Map* map) : _map(map)
{
    _timer = new Timer();
    addDependency<sWaveComponent>();
    addDependency<sPositionComponent>();
}

WaveSystem::~WaveSystem() {}

void    WaveSystem::update(EntityManager &em, float elapsedTime)
{
    bool reset = false;

    forEachEntity(em, [&](Entity *entity)
    {
        sWaveComponent* waveComponent = entity->getComponent<sWaveComponent>();
        sPositionComponent* position = entity->getComponent<sPositionComponent>();

        waveComponent->spawnPos = glm::vec3(position->value.x + 0.5f, position->value.y + 0.5f, position->z + 1);
        if (waveComponent->nSpawn > 0 && _timer->getElapsedTime() > waveComponent->secBeforeSpawn)
        {
            createEntity(_map, waveComponent->spawnPos, eArchetype::ENEMY);
            waveComponent->nSpawn -= 1;
            reset = true;
        }
    });
    if (reset)
        _timer->reset();
}

Entity*    WaveSystem::createEntity(Map* map, const glm::vec3& pos, eArchetype type)
{
    Entity* entity = EntityFactory::createEntity(type);

    sPositionComponent* posEntity = entity->getComponent<sPositionComponent>();
    posEntity->value.x = pos.x;
    posEntity->value.y = pos.y;
    posEntity->z = pos.z;

    (*map)[(uint16_t)pos.z].addEntity(entity->id);

    /*  static int idSoundSpawn = SoundManager::getInstance()->registerSound("resources/sounds/spawn.mp3", DEFAULT_SOUND);
    SoundManager::getInstance()->playSound(idSoundSpawn);*/

    return (entity);
}