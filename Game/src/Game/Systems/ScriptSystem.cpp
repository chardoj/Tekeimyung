/**
* @Author   Simon AMBROISE
*/

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <Game/Components.hh>
#include <Game/EntityFactory.hpp>

#include <Game/Systems/ScriptSystem.hpp>

ScriptSystem::ScriptSystem()
{
    this->addDependency<sScriptComponent>();

    _monitoringKey = MonitoringDebugWindow::getInstance()->registerSystem(SCRIPT_SYSTEM_NAME);
}

ScriptSystem::~ScriptSystem() {}

void    ScriptSystem::update(EntityManager &em, float elapsedTime)
{
    Timer timer;
    uint32_t nbEntities = 0;

    forEachEntity(em, [&](Entity *entity)
    {
        auto entityClass = entity->getComponent<sScriptComponent>()->scriptClass;

        entityClass->Update();
       
        ++nbEntities;
    });

    MonitoringDebugWindow::getInstance()->updateSystem(_monitoringKey, timer.getElapsedTime(), nbEntities);
}