#include "Backend.hpp"
#include "glm/ext/vector_float2.hpp"
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
#include <vector>

#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#endif

#include "inc/FastNoiseLite/Cpp/FastNoiseLite.h"


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
    engine.resources.CreateRectFromImage(tiles, "unity", {0, 192, 64, 48});
    engine.resources.CreateRectFromImage(tiles, "water", {64, 48, 64, 48});

    // Player
    engine.resources.LoadImage("player", "run.png");
    engine.resources.LoadImage("run_complete", "run_complete.png");
    engine.resources.LoadImage("run_complete2", "run_complete.png");

    
    engine.resources.LoadImage("grass-tile", "grass_tile.png");
    engine.resources.LoadImage("bg", "grid.png");
    
    Scene scene = Scene(engine);

  
    
    scene.Add({
        Sprite{"bg"},
        Position{0, 0},
        Color{255, 0 , 0}
    });

    
    float scale = 2.0f;
    glm::mat2 transform = glm::mat2(
        //glm::vec2(1 * (32/2.0), 0.5 * (23/2.0)),
        //glm::vec2(-1 * (32/2.0), 0.5 * (23/2.0))
        glm::vec2(16.0f, 8.0) * scale,
        glm::vec2(-16.0f, 8.0f) * scale

    );

    // Noise
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.05f);

    const int maxHeight = 48;
    //const float blockHeigh = 16.0f * scale;

    const int tiles_x = 12;
    const int tiles_y = 24;

    // It shouldn't be calculated manually, just for dev now
    const int offset_x = 750;
    const int offset_y = 250;
    
    for (int i = 0; i < tiles_x; i++)
    {
        for (int j = 0; j < tiles_y; j++)
        {

            std::vector<std::string> tiles{"grass", "lava", "brick", "sand", "marble", "water"};
            int index = rand() % tiles.size();
            std::string value = tiles[index];

            //int noise = rand() % 8

            float noiseAtPoint = noise.GetNoise(float(i), float(j));
            int height = (noiseAtPoint + 1.0f) * 0.5f * maxHeight; // from 0 to maxHeight
            //std::cout << height << std::endl;
            
            glm::vec2 res = transform * glm::vec2(i, j);

            auto& tile = scene.Add({
                Sprite{"grass"},
                Position{
                    res.x - (64.0f * scale)/2.0f + (tiles_x/2.0f) * 64 + offset_x,
                    res.y - height + offset_y
                },
                Scale{(int)scale}
            });

            
            float phase = (i + j) * 0.3f;
            float baseY = tile.GetComponent<Position>()->y;
            //float phase = 0.0f;
            //if (i == 9 && j == 0)

            tile.onUpdate = [&engine, &tile, baseY, phase](float dt) {
            
                float speed = 2.0f;
                float amplitude = 22.0f;
            
                //std::cout << tile.position.y << '\n';
            
                //tile.position.y = baseY + glm::sin(engine.time * speed + phase) * amplitude;
            };
            
        }
    }

    glm::mat2 inverseTransform = glm::mat2(
        glm::vec2(1, 0),
        glm::vec2(0, 1)
    );

    int i = 3;
    int j = 4;
    
    glm::vec2 res = transform * glm::vec2(i, j);
    
    //std::cout << mousePosUnity.x << " " << mousePosUnity.y << std::endl;

    float noiseAtPoint = noise.GetNoise(float(i), float(j));
    int height = (noiseAtPoint + 1.0f) * 0.5f * maxHeight; 
    
    auto& unity = scene.Add({
        Sprite{"unity"},
        Position{
            res.x - (64.0f * scale)/2.0f + (tiles_x/2.0f) * 64 + offset_x,
            res.y - height + offset_y
        },
        Color{255, 255, 0},
        Scale{(int)scale}
    });


    scene.Add({
        //Sprite{"player"},
        Tag{"Player"},
        AnimatedSprite{
            .source = "run_complete2",
            .size = glm::vec2(64),
            .currentAnimation = "run",
            .animations = 
                {
                    {"run", { .startY = 384,  .speed = 2.0f} }
                },
        },
        Position{590, 590},
        Scale{2}
    });

    
    scene.Add({
        Tag{"Enemy"},
        AnimatedSprite{
            .source = "run_complete",
            .size = glm::vec2(64),
            .currentAnimation = "run",
            .animations = 
                {
                    {"run", { .startY = 128,  .speed = 2.0f} }
                },
        },
        Position{100, 100},
        Scale{2}
    });
    
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
