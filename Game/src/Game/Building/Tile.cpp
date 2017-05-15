/*
** @Author : Simon AMBROISE
*/

#include <Engine/Core/Components/Components.hh>

#include <Engine/EntityFactory.hpp>
#include <Engine/Utils/ResourceManager.hpp>

#include <Game/Building/Tile.hpp>
#include <Game/Character/NewBuild.hpp>

void Tile::start()
{
    _render = this->getComponent<sRenderComponent>();
    _renderMaterial = *_render->getModelInstance()->getMeshsInstances()[0]->getMaterial();
    _buildMaterial = ResourceManager::getInstance()->getResource<Material>("build.mat");
}

void Tile::update(float dt) {}

void Tile::setBuildable(bool buildable)
{
    Material*   displayedMaterial;

    this->_buildable = buildable;
    displayedMaterial = (buildable == true ? this->_buildMaterial : &this->_renderMaterial);
    this->_render->getModelInstance()->setMaterial(displayedMaterial);
}

bool Tile::isBuildable()
{
    return this->_buildable;
}
