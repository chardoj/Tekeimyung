#pragma once

#include <vector>

#include "EntityManager.hpp"
#include "System.hpp"

class World
{
public:
    World();
    ~World();

    EntityManager&          getEntityManager();
    std::vector<System*>&   getSystems();

    template<typename T, typename... Args>
    void                    addSystem(Args... args)
    {
        _systems.push_back(new T(args...));
    }

    template<typename T>
    T*                      getSystem()
    {
        const std::type_info& typeInfo = typeid(T);

        for (auto system_: _systems)
        {
            if (system_->getTypeInfo().hash_code() == typeInfo.hash_code())
            {
                return (static_cast<T*>(system_));
            }
        }

        return (nullptr);
    }

private:
    EntityManager           _entityManager;
    std::vector<System*>    _systems;
};
