#include "../inc/Events.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>

bool Events::IsKeyDown(SDL_Scancode scancode)
{
    if (keyStates[scancode])
        return true;
    return false;
}

bool Events::IsButtonDown(SDL_Gamepad* gamepad, SDL_GamepadButton button)
{
    if (!gamepad)
        return false;

    return SDL_GetGamepadButton(gamepad, button);
}

float Events::GetAxisValue(SDL_Gamepad *gamepad, SDL_GamepadAxis axis)
{
    if (!gamepad)
        return 0;
    return SDL_GetGamepadAxis(gamepad, axis) / 32768.0;
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
