#ifndef BACKEND_HPP
#define BACKEND_HPP

#include "SDL3/SDL_video.h"
#include <SDL3/SDL_gamepad.h>
#include <cstddef>
#include <cstdint>
#include <webgpu/webgpu_cpp.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>

#define WIDTH 1920
#define HEIGHT 1080

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
    wgpu::CommandEncoder enconder;

    // SDL
    SDL_Window *window;
    SDL_Event events;
    SDL_Gamepad* gamepad = nullptr;
    SDL_DisplayID* displays = nullptr;
    int displaysCount = 0;
    uint64_t currentTime = 0;
    
    void Init();

    const bool* keys = nullptr;

    void DrawImage();

    wgpu::Device GetDevice();
    wgpu::Queue GetQueue();
    SDL_Event& GetEvents();
    wgpu::TextureFormat GetFormat();
    wgpu::CommandEncoder GetCommandEncoder();

    wgpu::Adapter RequestAdapter();
    wgpu::Device RequestDevice();
    wgpu::Surface RequestSurfaceFromSDL();

    void End();

    // Configurations
    void ConfigureSurface();



};

#endif
