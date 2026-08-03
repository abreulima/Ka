#ifndef LUA_RUNTIME_HPP
#define LUA_RUNTIME_HPP

#include "Components.hpp"
#include <sol/sol.hpp>

#include "Scene.hpp"

#include <string>

class Engine;

class LuaRuntime{

    public:
        void Init(Engine& engine);
        void LoadScript(const std::string& path);
        void RegisterBindings();
        void CallScene(const std::string& name);
        void LoadScene(const std::string& path, Scene& scene);
        std::vector<Components> ParseComponents(const sol::table& values);
    private:
        sol::state lua;
        Engine* engine = nullptr;
};

#endif