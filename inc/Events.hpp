#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <SDL3/SDL.h>

class Events
{
    private:
        const bool *keyStates;
    public:
        void Init();
        bool IsKeyDown(SDL_Scancode scancode);
        SDL_FPoint GetMousePoint();
};



#endif