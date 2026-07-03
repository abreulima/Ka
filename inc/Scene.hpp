#ifndef WORLD_HPP
#define WORLD_HPP

#include "Entity.hpp"
#include <memory>

class Engine;

class Scene
{
    private:
        Engine& engine;
        
    public:
        std::vector<std::shared_ptr<Entity>> entities;
        Scene(Engine& engine) : engine(engine)  {};
        Entity& Add(const std::vector<Components> &components);
};

#endif