/*
** @Author : Mathieu Chassara
*/

#include    <Engine/EntityFactory.hpp>
#include    <Engine/Physics/Collisions.hpp>

#include    <Game/Scripts/NewBuild.hpp>
#include    <Game/Scripts/Tile.hpp>
#include    <Game/Scripts/Player.hpp>

void        NewBuild::start()
{
    this->retrieveGameManager();
    this->bindEntitiesToInputs();
    this->retrievePlayerScript();
    this->_radius = 7.7f;
}

void        NewBuild::update(float deltaTime)
{
    this->checkUserInputs();
}

bool        NewBuild::isEnabled() const
{
    return (this->_enabled);
}

void        NewBuild::setEnabled(bool enabled)
{
    this->_enabled = enabled;
}

void        NewBuild::disableAll()
{
    this->_enabled = false;
    this->_currentChoice.erase();
    this->_player->setCanShoot(true);

    auto        em = EntityFactory::getBindedEntityManager();
    const auto& floorTiles = em->getEntitiesByTag("TileFloor");

    this->Destroy(this->_preview);
    this->_preview = nullptr;
    this->_tileHovered = nullptr;
    for (Entity* floorTile : floorTiles)
    {
        sScriptComponent*   scriptComponent = floorTile->getComponent<sScriptComponent>();

        if (scriptComponent != nullptr)
        {
            Tile*   tileScript = scriptComponent->getScript<Tile>("Tile");

            if (tileScript != nullptr)
                tileScript->setBuildable(false);
        }
    }
}

void        NewBuild::setTileHovered(const Entity* tileHovered)
{
    if (tileHovered != nullptr && this->_tileHovered != tileHovered)
    {
        this->Destroy(this->_preview);
        this->_preview = nullptr;

        this->_tileHovered = tileHovered;

        sScriptComponent*   scriptComponent = this->_tileHovered->getComponent<sScriptComponent>();

        if (scriptComponent == nullptr)
        {
            LOG_WARN("Could not find %s on Entity %s", "sScriptComponent", "TileFloor");
            return;
        }

        Tile*   tileScript = scriptComponent->getScript<Tile>("Tile");

        if (tileScript == nullptr)
        {
            LOG_WARN("Could not find script %s on Entity %s", "sScriptComponent", "TileFloor");
        }

        if (tileScript->isBuildable() == true)
        {
            auto&       position = this->_tileHovered->getComponent<sTransformComponent>()->getPos();

            if (this->_tileHovered->getTag() == "TileBaseTurret")
                this->_currentChoice = "TOWER_FIRE";
            else if (this->_tileHovered->getTag() == "TileFloor" && this->_currentChoice == "TOWER_FIRE")
                this->_currentChoice = "TILE_BASE_TURRET";
            this->_preview = this->Instantiate(this->_currentChoice, glm::vec3(position.x, position.y + 12.5f, position.z));

            if (this->_preview != nullptr)
            {
                auto    previewRenderer = this->_preview->getComponent<sRenderComponent>();
                auto    previewScripts = this->_preview->getComponent<sScriptComponent>();

                previewRenderer->ignoreRaycast = true;
                previewScripts->enabled = false;
            }
        }
    }
}

void        NewBuild::retrieveGameManager()
{
    auto    em = EntityFactory::getBindedEntityManager();
    Entity* gameManager = em->getEntityByTag("GameManager");

    if (gameManager == nullptr)
    {
        LOG_WARN("Could not find Entity with tag \"%s\"", "GameManager");
        return;
    }

    auto scriptComponent = gameManager->getComponent<sScriptComponent>();

    if (scriptComponent == nullptr)
    {
        LOG_WARN("Could not find %s on Entity %s", "sScriptComponent", "GameManager");
        return;
    }

    this->_gameManager = scriptComponent->getScript<GameManager>("GameManager");

    if (this->_gameManager == nullptr)
    {
        LOG_WARN("Could not find script %s on Entity %s", "sScriptComponent", "GameManager");
    }
}

void        NewBuild::retrievePlayerScript()
{
    auto    em = EntityFactory::getBindedEntityManager();
    Entity* player = em->getEntityByTag("Player");

    if (player == nullptr)
    {
        LOG_WARN("Could not find Entity with tag \"%s\"", "Player");
        return;
    }

    auto scriptComponent = player->getComponent<sScriptComponent>();

    if (scriptComponent == nullptr)
    {
        LOG_WARN("Could not find %s on Entity %s", "sScriptComponent", "Player");
        return;
    }

    this->_player = scriptComponent->getScript<Player>("Player");

    if (this->_player == nullptr)
    {
        LOG_WARN("Could not find script %s on Entity %s", "sScriptComponent", "Player");
    }
}

void        NewBuild::bindEntitiesToInputs()
{
    this->_bindedEntities.insert(std::make_pair(Keyboard::eKey::KEY_1, "TILE_BASE_TURRET"));
    //this->_bindedEntities.insert(std::make_pair(Keyboard::eKey::KEY_2, "TOWER_FIRE"));
    this->_bindedEntities.insert(std::make_pair(Keyboard::eKey::KEY_2, "TRAP_NEEDLE"));
    this->_bindedEntities.insert(std::make_pair(Keyboard::eKey::KEY_3, "TRAP_CUTTER"));
    this->_bindedEntities.insert(std::make_pair(Keyboard::eKey::KEY_4, "TRAP_FIRE"));
}

void        NewBuild::checkUserInputs()
{
    if (this->_enabled == true && this->mouse.getStateMap()[Mouse::eButton::MOUSE_BUTTON_1] == Mouse::eButtonState::CLICK_RELEASED)
        this->placePreviewedEntity();
    else if (this->_enabled == true && this->mouse.getStateMap()[Mouse::eButton::MOUSE_BUTTON_2] == Mouse::eButtonState::CLICK_RELEASED)
        this->disableAll();

    if (this->_currentChoice.empty() == true)
    {
        //  auto = std::pair<Keyboard::eKey, std::string>
        for (auto bindedEntity : this->_bindedEntities)
        {
            if (KB_P(bindedEntity.first) == true)
            {
                this->_currentChoice = bindedEntity.second;
                LOG_DEBUG("Current choice :\t%s", this->_currentChoice.c_str());
                break;
            }
        }
    }
    else
    {
        this->setEnabled(true);
        this->triggerBuildableZone(this->_currentChoice);
        this->_player->setCanShoot(false);
    }
}

void        NewBuild::triggerBuildableZone(const std::string &archetype)
{
    sTransformComponent*    transform = this->getComponent<sTransformComponent>();

    auto        em = EntityFactory::getBindedEntityManager();

    //  Looking for floor tiles
    const auto& floorTiles = em->getEntitiesByTag("TileFloor");
    for (Entity* floorTile : floorTiles)
    {
        auto    box = floorTile->getComponent<sBoxColliderComponent>();

        if (box != nullptr)
        {
            auto    tileTransform = floorTile->getComponent<sTransformComponent>();
            auto    tileScriptComponent = floorTile->getComponent<sScriptComponent>();

            if (tileScriptComponent == nullptr || tileScriptComponent->enabled == false)
                continue;

            Tile*   tileScript = tileScriptComponent->getScript<Tile>("Tile");

            if (tileScript == nullptr)
                continue;

            tileScript->setBuildable(false);

            if (this->isEnabled() == false)
                continue;

            if (Collisions::sphereVSAABB(transform->getPos(),
                this->_radius * SIZE_UNIT, box->pos + tileTransform->getPos(),
                glm::vec3(box->size.x * SIZE_UNIT, box->size.y * SIZE_UNIT, box->size.z * SIZE_UNIT)))
            {
                tileScript->setBuildable(true);
            }
        }
    }

    //  Looking for turret base tiles
    const auto& turretBasetiles = em->getEntitiesByTag("TileBaseTurret");
    for (Entity* turretBaseTile : turretBasetiles)
    {
        auto    box = turretBaseTile->getComponent<sBoxColliderComponent>();

        if (box != nullptr)
        {
            auto    tileTransform = turretBaseTile->getComponent<sTransformComponent>();
            auto    tileScriptComponent = turretBaseTile->getComponent<sScriptComponent>();

            if (tileScriptComponent == nullptr || tileScriptComponent->enabled == false)
                continue;

            Tile*   tileScript = tileScriptComponent->getScript<Tile>("Tile");

            if (tileScript == nullptr)
                continue;

            tileScript->setBuildable(false);

            if (this->isEnabled() == false)
                continue;

            if (Collisions::sphereVSAABB(transform->getPos(),
                this->_radius * SIZE_UNIT, box->pos + tileTransform->getPos(),
                glm::vec3(box->size.x * SIZE_UNIT, box->size.y * SIZE_UNIT, box->size.z * SIZE_UNIT)))
            {
                tileScript->setBuildable(true);
            }
        }
    }
}

void        NewBuild::placePreviewedEntity()
{
    auto&   position = this->_tileHovered->getComponent<sTransformComponent>()->getPos();
    auto    entity = this->Instantiate(this->_currentChoice, glm::vec3(position.x, position.y + 12.5f, position.z));


    if (entity->getTag() != "TileBaseTurret")
    {
        auto previewRenderer = entity->getComponent<sRenderComponent>();
        previewRenderer->ignoreRaycast = true;
    }

    glm::ivec2              tilePos;
    sTransformComponent*    tileTransform = this->_tileHovered->getComponent<sTransformComponent>();

    tilePos.x = static_cast<int>(std::ceil(tileTransform->getPos().x) / 25.0f);
    tilePos.y = static_cast<int>(tileTransform->getPos().z / 25.0f);
    this->_gameManager->firstLayerPattern[tilePos.x][tilePos.y] = 0;
    this->updateSpawnersPaths(tilePos);
}

void        NewBuild::updateSpawnersPaths(const glm::ivec2& tilePos)
{
    
}