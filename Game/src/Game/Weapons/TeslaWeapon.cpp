/*
** Author : Simon AMBROISE
*/

#include <vector>

#include <Engine/EntityFactory.hpp>
#include <Engine/Physics/Collisions.hpp>
#include <Engine/Physics/Physics.hpp>

#include <Game/Weapons/TeslaOrb.hpp>
#include <Game/Weapons/TeslaWeapon.hpp>
#include <Game/Character/Enemy.hpp>

const std::string TeslaWeapon::Name = "Tesla";

TeslaWeapon::TeslaWeapon()
{
    this->_attributes["FireRate"] = new Attribute(0.42f);
    this->_attributes["Ammo"] = new Attribute(100.0f);
    this->_attributes["Damage"] = new Attribute(12.0f);
    this->_attributes["MaxRange"] = new Attribute(35.0f);
    this->_attributes["HitAmount"] = new Attribute(2);
    this->_attributes["StunChance"] = new Attribute(0.0f);
    this->_attributes["ReloadingTime"] = new Attribute(0.9f);

    _shootSound = EventSound::getEventByEventType(eEventSound::PLAYER_SHOOT_ELEC);
    _material = ResourceManager::getInstance()->getResource<Material>("weapon_tesla.mat");
}

void    TeslaWeapon::fire(Player* player, sTransformComponent* playerTransform, sRenderComponent* playerRender, glm::vec3 playerDirection)
{
    this->_attributes["Ammo"]->addBonus(Bonus(-1));

    if (this->_attributes["Ammo"]->getFinalValue() == 0)
        this->_reloading = true;

    switch (this->_level)
    {
        case 0:
        case 1:
        case 2:
            this->fireLightning(player, playerTransform, playerRender, playerDirection);
            break;
        case 3:
            this->fireOrb(player, playerTransform, playerRender, playerDirection);
            break;
    }

#if (ENABLE_SOUND)
    if (_shootSound->soundID != -1)
    {
        SoundManager::getInstance()->playSound(_shootSound->soundID);
    }
#endif
}

void    TeslaWeapon::reload()
{
    this->_attributes["Ammo"]->clearAllBonuses();
}

void    TeslaWeapon::upgradeByLevel()
{
    switch (this->_level)
    {
        case 1:
            this->_attributes["Damage"]->addModifier(Modifier(100.0 / 100.0));
            this->_attributes["HitAmount"]->addBonus(Bonus(1));
            break;
        case 2:
            this->_attributes["StunChance"]->addModifier(Modifier(35.0 / 100.0));
            break;
        case 3:
            break;
        default:
            break;
    }
}

void    TeslaWeapon::fireLightning(Player* player, sTransformComponent* playerTransform, sRenderComponent* playerRender, glm::vec3 playerDirection)
{
    glm::vec3               playerPos;
    Ray                     raycastHit;
    std::vector<Entity*>    hitEntities;
    Entity*                 hitEntity;

    playerPos = playerTransform->getPos() + (playerRender->getModel()->getSize().y / 2.0f * playerTransform->getScale().y);
    raycastHit = Ray(playerPos, glm::vec3{ playerDirection.x, 0.0f, playerDirection.z });

    if (Physics::raycast(raycastHit, &hitEntity, std::vector<Entity*> { player->getEntity() }) == true &&
        hitEntity->getTag() == "Enemy")
    {
        this->spreadLightning(hitEntity, this->_attributes["HitAmount"]->getFinalValue());
    }
}

void    TeslaWeapon::spreadLightning(Entity* hitEntity, unsigned int hitLeft, std::vector<Entity*> entitiesAlreadyHit)
{
    if (hitLeft > 0)
    {
        auto    em = EntityFactory::getBindedEntityManager();
        auto&   enemies = em->getEntitiesByTag("Enemy");

        float   nearestDistance = std::numeric_limits<float>::max();
        Entity* nearestEntity = nullptr;
        Enemy*  nearestEnemy = nullptr;

        for (Entity* entity : enemies)
        {
            //  If the entity has not been hit by the lightning yet.
            if (std::find(entitiesAlreadyHit.begin(), entitiesAlreadyHit.end(), entity) == entitiesAlreadyHit.end())
            {
                sScriptComponent*   scriptComponent = entity->getComponent<sScriptComponent>();

                if (scriptComponent != nullptr)
                {
                    Enemy*  enemyScript = scriptComponent->getScript<Enemy>("Enemy");

                    if (enemyScript != nullptr)
                    {
                        sBoxColliderComponent*  box = entity->getComponent<sBoxColliderComponent>();

                        if (box != nullptr)
                        {
                            sTransformComponent*    hitEntityPos = hitEntity->getComponent<sTransformComponent>();
                            sTransformComponent*    entityPos = entity->getComponent<sTransformComponent>();

                            if (hitEntityPos != nullptr && entityPos != nullptr)
                            {
                                float   distance = Physics::distance(hitEntityPos->getPos(), entityPos->getPos());

                                if (nearestEntity == nullptr || distance < nearestDistance)
                                {
                                    nearestDistance = distance;
                                    nearestEntity = entity;
                                    nearestEnemy = enemyScript;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (nearestEntity != nullptr)
        {
            entitiesAlreadyHit.push_back(nearestEntity);
            nearestEnemy->takeDamage(this->_attributes["Damage"]->getFinalValue());
            this->triggerLightningEffect(nearestEntity);
            this->spreadLightning(nearestEntity, hitLeft - 1, entitiesAlreadyHit);
        }
    }
}

void    TeslaWeapon::triggerLightningEffect(Entity* entity)
{
    sScriptComponent*   scriptComponent = entity->getComponent<sScriptComponent>();

    if (scriptComponent != nullptr)
    {
        Enemy*              enemyScript = scriptComponent->getScript<Enemy>("Enemy");

        if (enemyScript != nullptr)
        {
            sRenderComponent*   render = entity->getComponent<sRenderComponent>();

            if (render != nullptr)
            {
                render->_animator.play("lightning_effect", false);
                
                auto& meshInstances = render->getModelInstance()->getMeshsInstances();

                for (auto& meshInstance : meshInstances)
                {
                    auto mat = meshInstance->getMaterial();
                    mat->setBloom(glm::vec4{ 0.0f, 0.0f, std::numeric_limits<float>::max() * 10.0f, mat->getBloom().w });
                }
            }
        }
    }
}

void            TeslaWeapon::fireOrb(Player* player, sTransformComponent* playerTransform, sRenderComponent* playerRender, glm::vec3 playerDirection)
{
    Entity*     teslaOrb = EntityFactory::createEntity("TESLA_ORB");

    if (teslaOrb == nullptr)
        throw std::runtime_error("Could not create Entity with archetype \"" + std::string("TESLA_ORB") + "\"");

    sScriptComponent*   scriptComponent = teslaOrb->getComponent<sScriptComponent>();

    if (scriptComponent == nullptr)
        throw std::runtime_error("Could not retrieve " + std::string("sScriptComponent") + " from Entity with archetype \"" + std::string("TESLA_ORB") + "\"");

    TeslaOrb*   orbScript = scriptComponent->getScript<TeslaOrb>("TeslaOrb");

    if (scriptComponent == nullptr)
        throw std::runtime_error("Could not retrieve script " + std::string("TeslaOrb") + " from Entity with archetype \"" + std::string("TESLA_ORB") + "\"");

    //  Let's set the orb's direction according to the player's.
    orbScript->setPosition(playerTransform->getPos());
    orbScript->setDirection({ playerDirection.x, 0.0f, playerDirection.z });
}

void    TeslaWeapon::clean()
{
}