#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <cstdint>
#include <map>

#include "Backend.hpp"
#include "Renderer.hpp"
#include "Resources.hpp"
#include "Scene.hpp"
#include "Events.hpp"

#define FIXED_DT 0.016666; // 60 FPS

class Engine
{
    private:
    public:
        bool isRunning;
        std::string currentScene;
        std::uint64_t lasTime;
        double acc = 0.0f;
        
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
        
        // Worlds
        std::map<std::string, Scene> scenes;
};

#endif