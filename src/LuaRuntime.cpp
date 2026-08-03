#include "../inc/LuaRuntime.hpp"
//#include "../inc/LuaBindings.hpp"
#include "../inc/Engine.hpp"
#include <stdexcept>
#include <string>

#include "../inc/Components.hpp"

void LuaRuntime::Init(Engine& engine) {

    this->engine = &engine;

    lua.open_libraries(
        sol::lib::base,
        sol::lib::math,
        sol::lib::table,
        sol::lib::string
    );

    RegisterBindings();
    
}

void LuaRuntime::RegisterBindings() {

    if (engine == nullptr) {
        throw std::runtime_error(
            "Engine not found!"
        );
    }

    lua.new_usertype<Entity>(
        "Entity",
        sol::no_constructor,

        "x",
        sol::property(
            [](const Entity& entity)
            {
                return entity.position.x;
            },
            [](Entity& entity, float value)
            {
                entity.position.x = value;
            }
        ),

        "y",
        sol::property(
            [](const Entity& entity)
            {
                return entity.position.y;
            },
            [](Entity& entity, float value)
            {
                entity.position.y = value;
            }
        ),

        "OnUpdate",
        [](Entity& entity, sol::protected_function callback)
        {
            entity.luaOnUpdate = std::move(callback);
        }
        
    );

    lua.set_function(
        "position",
        [](int x, int y) {
            return Position{x, y};
        }
    );

    lua.set_function(
        "sprite",
        [](std::string name) {
            return Sprite{std::move(name)};
        }
    );
    


    lua.set_function(
        "loadSprite",
        [this](const std::string& name, const std::string& path) {
            //engine->LoadSprite(name, path);
            engine->resources.LoadImage(name, path);
        }
    );    
    
}

void LuaRuntime::LoadScript(const std::string& path) {

    sol::protected_function_result result = lua.safe_script_file(path, sol::script_pass_on_error);

    if (!result.valid())
    {
        sol::error error = result;
        std::cerr << "Failed to load Lua script" << path << ":" << error.what() << std::endl;
    }
        
}


std::vector<Components> LuaRuntime::ParseComponents(const sol::table& values)
{
    std::vector<Components> components;
    components.reserve(values.size());

    for (std::size_t index = 1; index <= values.size(); ++index) {

        sol::object value = values[index];

        if (!value.valid() || value == sol::nil)
            continue ;

        if (value.is<Position>()) {
            components.emplace_back(value.as<Position>());
        }
        else if(value.is<Sprite>()) {
            components.emplace_back(value.as<Sprite>());
        }
        else
        {
            throw std::runtime_error(
                "Unknown component at index"  + std::to_string(index)
            );
        }
    }

    return components;
}

void LuaRuntime::LoadScene(
    const std::string& path,
    Scene& scene
)
{

    // 
    lua.set_function(
        "entity",
        [this, &scene](const sol::table& table) -> Entity& {

            std::vector<Components> components = ParseComponents(table);
            return scene.Add(std::move(components));
        }
    );

    sol::protected_function_result result = 
        lua.safe_script_file(
            path,
            sol::script_pass_on_error
        );

    if (!result.valid())
    {
        sol::error error = result;

        throw std::runtime_error(
            "Failed to load Lua scene" +
            path +
            " : " +
            error.what()
        );
    }
}