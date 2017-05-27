/*
** Author : Simon AMBROISE
*/

#pragma once

#include <map>

#include <Engine/Core/Components/Components.hh>
#include <Engine/Graphics/Material.hpp>
#include <Game/Attibutes/Attribute.hpp>
#include <Game/Character/Player.hpp>

class   IWeapon
{
    friend class Player;

public:
    virtual void fire(Player* player, sTransformComponent* playerTransform, sRenderComponent* playerRender, glm::vec3 playerDirection) = 0;
    virtual void reload() = 0;
    virtual double getAttribute(std::string attributeName)
    {
        return this->_attributes[attributeName]->getFinalValue();
    };

    virtual void upgradeByLevel() = 0;

    virtual void levelUp()
    {
        this->_level++;
        LOG_DEBUG("Weapon level up : %d", this->_level);
        this->upgradeByLevel();
    }

    virtual void update(float dt, Mouse& mouse) {}

    virtual std::string getName() = 0;

    virtual void clean() = 0;

    Material* getMaterial()
    {
        return (_material);
    }

protected:
    std::map<std::string, Attribute*> _attributes;
    Material* _material = nullptr;

    int _level = 0;
};
