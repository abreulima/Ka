#include "../inc/Engine.hpp"
#include "../inc/Backend.hpp"
//#include "../inc/LuaRuntime.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <cassert>

#include <cstdint>
#include <iostream>

void Engine::Start()
{
    isRunning = false;
    currentScene = "main";

    // .. stuffs

    backend.Init();

    assert(backend.GetQueue() != nullptr);
    assert(backend.GetDevice() != nullptr);

    renderer.Init(backend.GetDevice(), backend.GetQueue(), backend.GetFormat(), backend.surface);
    resources.Init(backend.GetDevice(), backend.GetQueue());
    events.Init();

    //luaRuntime.Init(*this);


    isRunning = true;
    lasTime = SDL_GetTicksNS();
}

void Engine::Update()
{

    //backend.UpdateEvents();
    //

    bool isMouseLeftClicked = false;

    while (SDL_PollEvent(&backend.GetEvents()))
    {
        if (backend.GetEvents().type == SDL_EVENT_QUIT)
            isRunning = false;

        if (backend.GetEvents().type == SDL_EVENT_KEY_DOWN)
        {
            if (backend.GetEvents().key.scancode == SDL_SCANCODE_ESCAPE)
                isRunning = false;
        }

        if (backend.GetEvents().type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
            backend.GetEvents().button.button == SDL_BUTTON_LEFT)
        {
            isMouseLeftClicked = true;
        }

        // Gamepadd added
        if (backend.GetEvents().type == SDL_EVENT_GAMEPAD_ADDED)
        {
            if(!backend.gamepad)
            {
                backend.gamepad = SDL_OpenGamepad(backend.GetEvents().gdevice.which);
                if (!backend.gamepad)
                {
                    std::cout << "Failed to open gamepad!" << SDL_GetError();
                }
                else
                {
                    std::cout << SDL_GetGamepadName(backend.gamepad) << std::endl;
                }
            }
        }

        // Gamepadd removed
        if (backend.GetEvents().type == SDL_EVENT_GAMEPAD_REMOVED)
        {
            if (backend.gamepad && (SDL_GetGamepadID(backend.gamepad)) == backend.events.gdevice.which)
            {
                SDL_CloseGamepad(backend.gamepad);
                backend.gamepad = nullptr;
            }
        }



        //if (backend.GetEvents().type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
        //    backend.GetEvents().button.button == SDL_BUTTON_LEFT)
    }

    std::uint64_t currenTime = SDL_GetTicksNS();
    std::uint64_t frameTime = currenTime - lasTime;
    lasTime = currenTime;

    float dt = static_cast<float>(frameTime) / 1000000000.0f;

    if (dt > 0.25f)
        dt = 0.25f;

    time += dt;

    if (OnUpdate)
        OnUpdate.value()();

    auto it = scenes.find(currentScene);
    if (it == scenes.end())
        return ;

    SDL_FPoint mousePoint = events.GetMousePoint();

    for (auto &e : it->second.entities)
    {

        if (e->onUpdate)
            e->onUpdate.value()(dt);

        // Forgotten for now
        //e->LuaOnUpdate(dt);


        SDL_FRect entityFRect = e->GetPositionRect();
        bool isMouseOver = SDL_PointInRectFloat(&mousePoint, &entityFRect);

        if (isMouseLeftClicked)
        {
            std::cout << mousePoint.x << " " << mousePoint.y << std::endl;
        }

        // Hover
        if (isMouseOver && !e->isHovered)
        {
            e->isHovered = true;
            if (e->onHover)
                e->onHover.value()();
        }

        // Click
        if (e->onClick && isMouseOver && isMouseLeftClicked)
        {
            e->onClick.value()();
        }

        // End Hover
        if (!isMouseOver && e->isHovered)
        {
            e->isHovered = false;
            if (e->onHoverEnd)
                e->onHoverEnd.value()();
        }

    }

    if (OnLateUpdate)
        OnLateUpdate.value()();

}

void Engine::Render()
{
    auto it = scenes.find(currentScene);
    if (it == scenes.end())
        return ;

    renderer.Render(it->second.entities, camera.position);
}

void Engine::End()
{
    backend.End();
}
