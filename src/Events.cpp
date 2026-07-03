#include "../inc/Events.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_rect.h>

bool Events::IsKeyDown(SDL_Scancode scancode)
{
    if (keyStates[scancode])
        return true;
    return false;
}

SDL_FPoint Events::GetMousePoint()
{
    float x;
    float y;
    SDL_GetMouseState(&x, &y);
    return SDL_FPoint{x, y};
}

void Events::Init()
{
    keyStates = SDL_GetKeyboardState(nullptr);
}