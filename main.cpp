#include "Backend.hpp"
#include "inc/Components.hpp"
#include "inc/Engine.hpp"
#include "inc/Renderer.hpp"
#include "inc/Scene.hpp"

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_misc.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_scancode.h>

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

    // External Logic

    engine.resources.LoadImage("Milk", "milk.png");
    engine.resources.LoadImage("checker", "full-checker.png");
    engine.resources.LoadImage("platform", "platform.png");
    engine.resources.LoadImage("slime", "slime-2.png");
    engine.resources.LoadImage("grass", "grass.png");
    engine.resources.LoadImage("grass-bot", "grass-bot.png");
    engine.resources.LoadImage("grass-right", "grass-right.png");
    engine.resources.LoadImage("grass-left", "grass-left.png");
    engine.resources.LoadImage("grass-right-middle", "grass-right-middle.png");
    engine.resources.LoadImage("grass-left-middle", "grass-left-middle.png");

    engine.resources.LoadFont("monogram", "res/fonts/monogram-extended.ttf", 16);

    //engine.luaRuntime.LoadScript("_game/ASTRID_01/scripts/main.lua");

    Scene scene = Scene(engine);

    scene.Add({
        Sprite{"checker"},
        Position{0, 0},
        Scale{2}
    });

    auto& enemy = scene.Add({
        Sprite{"Milk"},
        Position{200, 300},
        Scale{2, 2}
    });

    auto& player = scene.Add({
        Sprite{"slime"},
        Position{0, 38},
        Scale{2, 2},
        Area{ .shape = {0, 0, 0, 0}},
        Platformer{},
    });

    scene.Add({
        Sprite{"Milk"},
        Position{0, 0},
        Scale{2, 2},
        Layer{LayerType::UI}
    });

    scene.Add({
        Sprite{"grass-left"},
        Position{0, 300},
        Scale{2}
    });

    scene.Add({
        Sprite{"grass-left-middle"},
        Position{0, 332},
        Scale{2}
    });

    scene.Add({
        Sprite{"grass-right"},
        Position{51 * 32, 300},
        Scale{2}
    });

    scene.Add({
        Sprite{"grass-right-middle"},
        Position{51* 32, 332},
        Scale{2}
    });


    scene.Add({
        Position{0, 0},
        //Sprite{"Milk"},
        Line{glm::vec2(0, 0), glm::vec2(100, 100)},
        //Fixed{},
        //Scale{2}
    });

    scene.Add({
        Position{WIDTH / 2, HEIGHT / 2},
        Rect{32 * 4, 32 * 4},
        //Fixed{},
    });

    scene.Add({
        Position{100, 100},
        Text("Please, subscribe!"),
        Layer{LayerType::FG},
        Scale{3}
    });


    for (int i = 0 ; i < 50; i++)
    {
        scene.Add({
            Sprite{"grass"},
            Position{32 + i * 32, 300},
            Scale{2}
        });

        scene.Add({
            Sprite{"grass-bot"},
            Position{32 + i * 32, 332},
            Scale{2}
        });

    }

    enemy.onUpdate = [&enemy](float dt){
        enemy.position.x = enemy.position.x + 80*dt;

    };

    float groundY = 300.0f - 64;

    player.onUpdate = [&engine, &player, groundY](float dt){

        SDL_Gamepad* gamepad = engine.backend.gamepad;

        auto platformer = player.GetComponent<Platformer>();
        if (!platformer) return ;

        if (
            engine.events.IsKeyDown(SDL_SCANCODE_LEFT) ||
            engine.events.IsButtonDown(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT) ||
            engine.events.GetAxisValue(gamepad, SDL_GAMEPAD_AXIS_LEFTX) > 0.2f
        )
        {
            platformer->velocity.x = -platformer->moveSpeed;
            player.Flip(true);
        }
        else if (
            engine.events.IsKeyDown(SDL_SCANCODE_RIGHT) ||
            engine.events.IsButtonDown(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT) ||
            engine.events.GetAxisValue(gamepad, SDL_GAMEPAD_AXIS_LEFTX) > 0.2f
        )
        {
            platformer->velocity.x = +platformer->moveSpeed;
            player.Flip(false);
        }
        else
            platformer->velocity.x = 0;


        if (
            (
                engine.events.IsKeyDown(SDL_SCANCODE_SPACE) ||
                engine.events.IsButtonDown(gamepad, SDL_GAMEPAD_BUTTON_SOUTH)
            )
            && platformer->isGrounded)
        {
            platformer->velocity.y = -platformer->jumpSpeed;
            platformer->isGrounded = false;
        }

        platformer->velocity.y += platformer->gravity * dt;
        player.position += platformer->velocity * dt;


        SDL_FRect playerRect = player.GetPositionRect();

        if (playerRect.y + playerRect.h >= groundY)
        {
            player.position.y = groundY - playerRect.h;
            platformer->velocity.y = 0;
            platformer->isGrounded = true;
        }
        else {
            platformer->isGrounded = false;
        }

        engine.camera.position = player.position - glm::vec2(WIDTH / 2.0f, HEIGHT / 2.0f);

    };

    //player.




    //engine.luaRuntime.LoadScene("_game/ASTRID_01/scripts/main.lua", scene);


    /*
        scene.Add({
        Sprite{"Milk"},
        Position{100, 100}
    });
    */

    engine.scenes.emplace("main", std::move(scene));

    // End

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
