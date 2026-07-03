#include "../inc/Engine.hpp"
#include "../inc/Backend.hpp"

#include <SDL3/SDL_events.h>
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
        
        //if (backend.GetEvents().type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
        //    backend.GetEvents().button.button == SDL_BUTTON_LEFT)
    }
    
    std::uint64_t currenTime = SDL_GetTicksNS();
    std::uint64_t frameTime = currenTime - lasTime;
    lasTime = currenTime;
    
    float dt = static_cast<float>(frameTime) / 1000000000.0f;
    
    if (dt > 0.25f)
        dt = 0.25f;
        
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
        
        SDL_FRect entityFRect = e->GetPositionRect();
        bool isMouseOver = SDL_PointInRectFloat(&mousePoint, &entityFRect);
        
        //std::cout << mousePoint.x << " " << mousePoint.y <<  std::endl;
        //std::cout << entityFRect.x << " " << entityFRect.y <<  std::endl;
        //std::cout << entityFRect.w << " " << entityFRect.h <<  std::endl;
        
        //std::cout << SDL_PointInRectFloat(&mousePoint, &entityFRect) << std::endl;
        
        
        // Hover 
        if (e->onHover && isMouseOver)
        {
            e->onHover.value()();
        }
        
        // Click
        if (e->onClick && isMouseOver && isMouseLeftClicked)
        {
            e->onClick.value()();
        }
        
        // End Hover
        if (e->onHoverEnd && !isMouseOver)
        {
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
    
    renderer.Render(it->second.entities);
}

void Engine::End()
{
    backend.End();
}
