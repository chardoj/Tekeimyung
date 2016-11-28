#pragma once

#include <unordered_map>

#include <ECS/System.hpp>

#include <Engine/Utils/MonitoringDebugWindow.hpp>
#include <Engine/Graphics/ShaderProgram.hpp>
#include <Engine/Graphics/Sprite.hpp>

#include <Game/Components.hh>
#include <Game/Map.hpp>
#include <Game/Systems/ParticleSystem.hpp>

#define RENDERING_SYSTEM_NAME "Rendering system"

class RenderingSystem: public System
{
public:
    RenderingSystem(Map* map, std::unordered_map<uint32_t, sEmitter*>* particleEmitters);
    virtual ~RenderingSystem();

    virtual void update(EntityManager& em, float elapsedTime);
    virtual bool                            init();
    void                                    renderEntity(Entity* entity);
    void                                    renderEntities(EntityManager& em, std::list<uint32_t>::const_iterator& it, uint16_t layer, uint32_t x, uint32_t y);
    void                                    renderParticles(EntityManager& em);
    const ShaderProgram&                    getShaderProgram() const;

private:
    Sprite*                                 getSprite(Entity* entity);
    void                                    getSpriteCreateInfo(Sprite::sCreateInfo& createInfo, sRenderComponent *sprite);

private:
    // Shader program
    ShaderProgram                               _shaderProgram;

    Map*                                        _map;
    std::unordered_map<uint32_t, sEmitter*>*    _particleEmitters;
    uint16_t                                    _keyMonitoring;
    tMonitoring                                 _data;
};
