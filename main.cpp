#include "Backend.hpp"
#include "glm/trigonometric.hpp"
#include "inc/Components.hpp"
#include "inc/Engine.hpp"
#include "inc/Renderer.hpp"
#include "inc/Scene.hpp"

#include <iostream>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_misc.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_scancode.h>
#include <cstdint>
#include <cstdlib>

#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#endif


/*
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
    engine.resources.LoadImage("checker", "full-checker.png");

    Scene scene = Scene(engine);

    scene.Add({
        Sprite{"checker"},
        Position{0, 0},
        Scale{2, 2},
    });

    auto &test = scene.Add({
        Sprite{"Slime"},
        Position{300, 300},
        Scale{4, 2},
    });

    test.onUpdate = [&test](float dt ){
        test.position.x += 5 * dt;
        test.position.y += 5 * dt;
    };

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
        Scale{2, 2},
        Position{100, 100},
        Anchor{AnchorPosition::CENTER},
        Layer{LayerType::UI},
        //Area{ .shape = {8, 8, 4, 4} },

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
        //youtube.scale = glm::vec2(2);

        std::cout << "OnHover" << std::endl;
    };

    youtube.onHoverEnd = [&youtube](){
        std::cout << "OnHoverEnd" << std::endl;
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
*/


int main()
{
    Engine engine = Engine();
    engine.Start();

    // Game Logic

    Image tiles = engine.resources.LoadImage("tiles", "isometric.png");
    engine.resources.CreateRectFromImage(tiles, "grass", {0, 0, 64, 48});
    engine.resources.CreateRectFromImage(tiles, "lava", {63, 0, 64, 48});
    engine.resources.CreateRectFromImage(tiles, "brick", {128, 96, 64, 48});
    engine.resources.CreateRectFromImage(tiles, "sand", {0, 144, 64, 48});
    engine.resources.CreateRectFromImage(tiles, "marble", {128, 144, 64, 48});

    engine.resources.LoadImage("grass-tile", "grass_tile.png");

    Scene scene = Scene(engine);

    float scale = 2.0f;
    glm::mat2 transform = glm::mat2(
        //glm::vec2(1 * (32/2.0), 0.5 * (23/2.0)),
        //glm::vec2(-1 * (32/2.0), 0.5 * (23/2.0))
        glm::vec2(16.0f, 8.0) * scale,
        glm::vec2(-16.0f, 8.0f) * scale

    );

    
    auto& teste = scene.Add({
        Sprite{"grass"},
        Position{300, 300},
        Scale{(int)scale}   
    });

    teste.onUpdate = [&engine](float dt){
        //std::cout << "Hello World!" << std::endl;
    };
    

    
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {

            std::vector<std::string> tiles{"grass", "lava", "brick", "sand", "marble"};
            int index = rand() % tiles.size();
            std::string value = tiles[index];

            //int noise = rand() % 8
            
            glm::vec2 res = transform * glm::vec2(i, j);

            auto& tile = scene.Add({
                Sprite{value},
                Position{
                    res.x - (64.0f * scale)/2.0f, 
                    res.y
                },
                Scale{(int)scale}
            });

            float baseY = res.y;
            float phase = (i + j) * 0.3f;

            if (i == 9 && j == 0)
            {
                    
                tile.onUpdate = [&engine, &tile, baseY, phase](float dt) {
        
                    float speed = 0.5f;
                    float amplitude = 3.0f;
        
                    std::cout << tile.position.y << '\n';
                    
                    tile.position.y = baseY + glm::sin(engine.time * speed + phase) * amplitude;  
                };
            }
            
        }

    }

    // End of Game Logic

    engine.scenes.emplace("main", std::move(scene));

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

}
