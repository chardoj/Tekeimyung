/**
* @Author   Simon Ambroise
*/

#include <Engine/Components.hh>

#include <Engine/Systems/RigidBodySystem.hpp>

RigidBodySystem::RigidBodySystem()
{
    this->addDependency<sRigidBodyComponent>();
    this->addDependency<sTransformComponent>();
}

void RigidBodySystem::update(EntityManager &em, float elapsedTime)
{
    this->forEachEntity(em, [&](Entity* entity) {
        sRigidBodyComponent* rigidBody = entity->getComponent<sRigidBodyComponent>();
        sTransformComponent* transform = entity->getComponent<sTransformComponent>();

        handleCollisions(em, entity, rigidBody);

        rigidBody->velocity += rigidBody->gravity * elapsedTime;
        transform->pos += rigidBody->velocity * elapsedTime;
        transform->needUpdate = true;
    });
}

void RigidBodySystem::handleCollisions(EntityManager &em, Entity* entity, sRigidBodyComponent* rigidBody)
{
    sScriptComponent* scriptComponent = entity->getComponent<sScriptComponent>();

    for (auto& it = rigidBody->collisions.begin(); it != rigidBody->collisions.end(); ++it)
    {
        if (it->second == eCollisionState::ENTERING_COLLISION)
        {
            if (scriptComponent != nullptr)
            {
                for (auto&& script : scriptComponent->scriptInstances)
                {
                    script->OnCollisionEnter(em.getEntity(it->first));
                }
            }
            rigidBody->collisions[it->first] = eCollisionState::IS_COLLIDING;
        }
        else if (it->second == eCollisionState::EXIT_COLLISION)
        {
            if (scriptComponent != nullptr)
            {
                for (auto&& script : scriptComponent->scriptInstances)
                {
                    script->OnCollisionExit(em.getEntity(it->first));
                }
            }
            rigidBody->collisions[it->first] = eCollisionState::NO_COLLISION;
        }
    }
}