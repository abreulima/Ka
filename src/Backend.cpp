#include "../inc/Backend.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

void Backend::Init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD);
    TTF_Init();
    
    window = SDL_CreateWindow(
        "NK",
        WIDTH, 
        HEIGHT, 
        0
    );
    
    if(!window)
    {
        std::cout << SDL_GetError() << std::endl;
        return ;
    }
    
    /* Start of Request Instance */
    std::vector<wgpu::InstanceFeatureName> requiredFeatures =
    {
        wgpu::InstanceFeatureName::TimedWaitAny
    };
    
    wgpu::InstanceDescriptor instanceDesc = {};
    instanceDesc.nextInChain = nullptr;
    instanceDesc.requiredFeatureCount = requiredFeatures.size();
    instanceDesc.requiredFeatures = requiredFeatures.data();
    
    instance = wgpu::CreateInstance(&instanceDesc);
    assert(instance != nullptr);
    /* End of Request Instance  */

    surface = RequestSurfaceFromSDL();
    assert(surface != nullptr);
    
    adapter = RequestAdapter();
    assert(adapter != nullptr);
    
    device = RequestDevice();
    assert(device != nullptr);
    
    queue = device.GetQueue();
    assert(queue != nullptr);
    
   
    // Configuration Calls
    ConfigureSurface();
}


wgpu::Surface Backend::RequestSurfaceFromSDL()
{
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    
    #if defined (SDL_PLATFORM_LINUX)
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
    {
        void *x11Display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
        uint64_t x11Window = SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, NULL);
        
        if (!x11Display || !x11Window)
        {
            std::cerr << SDL_GetError() << std::endl;
            return nullptr;
        }
        
        wgpu::SurfaceSourceXlibWindow fromXlibSurface = {};
        fromXlibSurface.nextInChain = nullptr;
        fromXlibSurface.sType = wgpu::SType::SurfaceSourceXlibWindow;
        fromXlibSurface.display = x11Display;
        fromXlibSurface.window = x11Window;
        
        wgpu::SurfaceDescriptor surfaceDesc = {};
        surfaceDesc.nextInChain = &fromXlibSurface;
        surfaceDesc.label = "X11 Surface";
        return instance.CreateSurface(&surfaceDesc);
    }
    #elif defined(SDL_PLATFORM_EMSCRIPTEN)
    
    wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector fromCanvasHTMLSelector = {};
    fromCanvasHTMLSelector.nextInChain = nullptr;
    fromCanvasHTMLSelector.sType = wgpu::SType::EmscriptenSurfaceSourceCanvasHTMLSelector;
    fromCanvasHTMLSelector.selector = "#canvas";
    
    wgpu::SurfaceDescriptor surfaceDesc = {};
    surfaceDesc.nextInChain = &fromCanvasHTMLSelector;
    surfaceDesc.label = "Canvas";
    
    return instance.CreateSurface(&surfaceDesc);
    
    #endif
    
    return nullptr;
}

wgpu::Adapter Backend::RequestAdapter()
{
    wgpu::RequestAdapterOptions requestAdapterOptions = {};
    requestAdapterOptions.compatibleSurface = surface;
    // requestAdapterOptions.backendType = wgpu::BackendType::Vulkan;

    wgpu::Adapter adapter;
    
    wgpu::Future f = this->instance.RequestAdapter(
        &requestAdapterOptions,
        wgpu::CallbackMode::WaitAnyOnly,
        [&adapter](
            wgpu::RequestAdapterStatus status,
            wgpu::Adapter _adapter,
            wgpu::StringView message
        )
        {
            if (status != wgpu::RequestAdapterStatus::Success)
            {
                std::cerr << "Failed to get an adapter" << message.data << std::endl;
                return ;
            }
            adapter = std::move(_adapter);
        }
    );
    
    this->instance.WaitAny(f, UINT64_MAX);
    if (!adapter)
        return nullptr;
    return adapter;
}

wgpu::Device Backend::RequestDevice()
{
    wgpu::DeviceDescriptor deviceDesc = {};
    deviceDesc.SetUncapturedErrorCallback([](
        const wgpu::Device&,
        wgpu::ErrorType errorType,
        wgpu::StringView message
    )
    {
        std::cerr << message.data << std::endl;
    });
    
    deviceDesc.SetDeviceLostCallback(
        wgpu::CallbackMode::AllowSpontaneous,
        [](
            const wgpu::Device& device,
            wgpu::DeviceLostReason reason,
            wgpu::StringView message
        )
        {
            // Program exited
            if (reason == wgpu::DeviceLostReason::Destroyed)
                return ;
            std::cerr << "Device lost due to " << message.data << std::endl;
        }
    );
    
    wgpu::Device device;
    
    wgpu::Future f = this->adapter.RequestDevice(
        &deviceDesc,
        wgpu::CallbackMode::WaitAnyOnly,
        [&device](
            wgpu::RequestDeviceStatus status,
            wgpu::Device _device,
            wgpu::StringView message
        )
        {
            if (status != wgpu::RequestDeviceStatus::Success)
            {
                std::cerr << "Failed to get a device " << message.data << std::endl;
                return ;
            }
            device = std::move(_device);
        }
    );
    
    this->instance.WaitAny(f, UINT64_MAX);
    if (!device)
        return nullptr;
    return device;
}
void Backend::ConfigureSurface()
{
    wgpu::SurfaceCapabilities surfaceCap = {};
    this->surface.GetCapabilities(this->adapter, &surfaceCap);
    this->format = surfaceCap.formats[0];
    
    wgpu::SurfaceConfiguration surfaceConfig = {};
    surfaceConfig.nextInChain = nullptr;
    surfaceConfig.width = WIDTH;
    surfaceConfig.height = HEIGHT;
    surfaceConfig.usage = wgpu::TextureUsage::RenderAttachment;
    surfaceConfig.format = this->format;
    surfaceConfig.alphaMode = surfaceCap.alphaModes[0];
    surfaceConfig.viewFormatCount = 0;
    surfaceConfig.viewFormats = nullptr;
    surfaceConfig.device = this->device;
    surface.Configure(&surfaceConfig);
}

wgpu::Device Backend::GetDevice()
{
    if (device)
        return device;
    return nullptr;
}

wgpu::Queue Backend::GetQueue()
{
    if (queue)
        return queue;
    return nullptr;
}

SDL_Event& Backend::GetEvents()
{
    return events;
}

wgpu::TextureFormat Backend::GetFormat()
{
    return format;
}
void Backend::End()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}
