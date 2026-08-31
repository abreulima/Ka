#ifndef BACKEND_HPP
#define BACKEND_HPP

#include <SDL3/SDL_gamepad.h>
#include <webgpu/webgpu_cpp.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>

#define WIDTH 960
#define HEIGHT 540

class Backend
{
public:

    // WebGPU
    wgpu::Instance instance;
    wgpu::Device device;
    wgpu::Queue queue;
    wgpu::Surface surface;
    wgpu::Adapter adapter;
    wgpu::TextureFormat format;

    // SDL
    SDL_Window *window;
    SDL_Event events;
    SDL_Gamepad* gamepad = nullptr;

    void Init();

    const bool* keys = nullptr;

    void DrawImage();

    wgpu::Device GetDevice();
    wgpu::Queue GetQueue();
    SDL_Event& GetEvents();
    wgpu::TextureFormat GetFormat();

    wgpu::Adapter RequestAdapter();
    wgpu::Device RequestDevice();
    wgpu::Surface RequestSurfaceFromSDL();

    void End();

    // Configurations
    void ConfigureSurface();



};

#endif
