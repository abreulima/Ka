#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <map>

#include "Backend.hpp"
#include "Renderer.hpp"
#include "Resources.hpp"
#include "Scene.hpp"
#include "Events.hpp"
#include "LuaRuntime.hpp"

constexpr double FIXED_DT = 1.0 / 60.0;

struct Camera {
    glm::vec2 position = {0.0f, 0.0f};
};

class Engine
{
    private:
    public:
        bool isRunning;
        std::string currentScene;
        std::uint64_t lasTime;
        double acc = 0.0f;
        double time = 0.0f;
        
        void Start();
        void Update();
        void Render();
        void End();
        
        // Called after everything done update
        std::optional<std::function<void (void)>> OnLateUpdate = std::nullopt;
        
        // Called before everything started updating
        std::optional<std::function<void (void)>> OnUpdate = std::nullopt;       
        
        Backend backend;
        Resources resources;
        Renderer renderer;
        Events events;
        LuaRuntime luaRuntime;
        
        // Worlds
        std::map<std::string, Scene> scenes;
        Camera camera;
        
};

#endif