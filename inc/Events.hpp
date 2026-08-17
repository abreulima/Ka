#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>

class Events
{
    private:
        const bool *keyStates;
    public:
        void Init();
        bool IsKeyDown(SDL_Scancode scancode);
        bool IsButtonDown(SDL_Gamepad* gamepad, SDL_GamepadButton button);
        SDL_FPoint GetMousePoint();

        float GetAxisValue(SDL_Gamepad *game, SDL_GamepadAxis axis);
        
        
};



#endif