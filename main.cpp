#include "inc/Components.hpp"
#include "inc/Engine.hpp"
#include "inc/Renderer.hpp"
#include "inc/Scene.hpp"
#include "inc/glm/trigonometric.hpp"

#include <SDL3/SDL_misc.h>
#include <SDL3/SDL_scancode.h>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

int main()
{
    // I don't like the other style
    Engine engine = Engine();
    engine.Start();
    
    engine.resources.LoadImage("Milk", "milk.png");
    engine.resources.LoadImage("Map", "map.png");
    engine.resources.LoadImage("Coin", "coin.png");
    engine.resources.LoadImage("Slime", "slime-2.png");
    engine.resources.LoadImage("YouTube", "youtube.png");
    engine.resources.LoadImage("Play", "play.png");
    engine.resources.LoadImage("HowToPlay", "how_to_play.png");
    engine.resources.LoadImage("Game", "name.png");
    
    Scene scene = Scene(engine);
    
    auto& name = scene.Add({
       Sprite{"Game"},
       Anchor{AnchorPosition::CENTER},
       Layer{LayerType::UI},
       Scale{2, 2},
       Position{960 / 2, 120}
    });
    
    
    name.onUpdate = [&engine, &name](float dt)
    {
        name.rotation = std::sin(engine.time) * 5.0f;
    };
    
    auto& play = scene.Add({
        Sprite{"Play"},
        Scale{2, 2},
        Position{960 / 2, 540 / 2 + 60},
        Anchor{AnchorPosition::CENTER},
        Layer{LayerType::UI}
    });
    
    auto& howToPlay = scene.Add({
        Sprite{"HowToPlay"},
        Scale{2, 2},
        Position{960 / 2, 540 / 2 + 160},
        Anchor{AnchorPosition::CENTER},
        Layer{LayerType::UI}
    });
    
    auto& player = scene.Add({
        Tag{"Hello"},
        Sprite{"Slime"},
        Scale{2, 2},
        Position{300, 300},
        Data{{
            {"lives", 3}
        }}
    });
    
    auto& youtube = scene.Add({
        Tag{"Button"},
        Sprite{"YouTube"},
        Scale{4, 4},
        Position{100, 100},
        Anchor{AnchorPosition::CENTER},
        Layer{LayerType::UI},
        Area{ .shape = {8, 8, 4, 4} },
        
    });
    
    auto& milk = scene.Add({
        Tag{"Player"},
        Sprite{"Milk"},
        Scale{2, 2},
        Color{255, 200, 103},
        // Opacity
        Position{324, 324}
    });
    
    float baseY = milk.position.y;
    milk.onUpdate = [&engine, &milk, &baseY](float dt){
        milk.position.y = baseY - glm::sin(engine.time * 2.0f) * 5.0f;
    };
    
    
    youtube.onClick = []()
    {
        //SDL_OpenURL("http:://google.com");
        std::cout << "Hello World" << std::endl;
    };
    
    youtube.onHover = [&youtube](){
        youtube.scale = glm::vec2(2);
    };
    
    //youtube.onHoverEnd = [&youtube](){
    //    youtube.scale = glm::vec2(2);
    //};
    
    player.onUpdate = [&engine, &player](float dt)
    {
        //std::cout << "Hello World!" << std::endl;
        //player.position += 1;
        //engine.
        const float SPEED = 200.0f;
        const float JUMP_SPEED = 400.0f;
        
        if (engine.events.IsKeyDown(SDL_SCANCODE_LEFT))
        {
            player.Flip(true);
            player.position.x -= SPEED * dt;
        }
        
        if (engine.events.IsKeyDown(SDL_SCANCODE_RIGHT))
        {
            player.Flip(false);
            player.position.x += SPEED * dt;
        }
            
        if (engine.events.IsKeyDown(SDL_SCANCODE_SPACE))
            player.position.y -= JUMP_SPEED * dt;
 
    };
    
    engine.scenes.emplace("main", scene);
    //engine.scenes["main"] = scene;
    
    #ifdef __EMSCRIPTEN__
    
    emscripten_set_main_loop_arg(
        [](void *arg)
        {
            Engine& engine = *reinterpret_cast<Engine*>(arg);
            engine.Update();
            engine.Render();
        },
        &engine, 
        0, 
        true);
    
    #else 
    
    while (engine.isRunning)
    {
        engine.Update();
        engine.Render();
    }
    
    #endif
    
    
    engine.End();
 
    return 0;
}
