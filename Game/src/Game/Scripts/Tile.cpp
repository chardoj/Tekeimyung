/*
** @Author : Simon AMBROISE
*/

#include <Engine/Components.hh>

#include <Engine/EntityFactory.hpp>
#include <Engine/Utils/ResourceManager.hpp>

#include <Game/Scripts/Tile.hpp>
#include <Game/Scripts/Build.hpp>

void Tile::start()
{
    _render = this->getComponent<sRenderComponent>();
    _renderMaterial = _render->getModelInstance()->getMeshsInstances()[0]->getMaterial();
    _buildMaterial = ResourceManager::getInstance()->getResource<Material>("build.mat");
}

void Tile::update(float dt)
{
}

void Tile::onHoverEnter()
{
    auto em = EntityFactory::getBindedEntityManager();

    auto buildScript = em->getEntityByTag("Player")->getComponent<sScriptComponent>()->getScript<Build>("Build");
    buildScript->setTile(this->entity);
}

void Tile::onHoverExit()
{
    auto em = EntityFactory::getBindedEntityManager();

    auto buildScript = em->getEntityByTag("Player")->getComponent<sScriptComponent>()->getScript<Build>("Build");
    buildScript->setTile(nullptr);
}

void Tile::setBuildable(bool buildable)
{
    this->_buildable = buildable;

    if (_buildable == true)
    {
        _render->getModelInstance()->setMaterial(_buildMaterial);
    }
    else
    {
        _render->getModelInstance()->setMaterial(_renderMaterial);
    }
}

bool Tile::isBuildable()
{
    return this->_buildable;
}
