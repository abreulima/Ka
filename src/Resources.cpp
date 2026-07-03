#include "../inc/Resources.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>

Image Resources::LoadImage(std::string name, std::string path)
{
    
    //assert(device != nullptr && queue != nullptr);
    //assert(3 == 5);
    
    SDL_Surface *loaded = SDL_LoadPNG(("res/sprites/" + path).c_str());
    if (!loaded)
    {
        std::cerr << SDL_GetError() << std::endl;
        std::abort();
    }
    
    SDL_Surface *converted = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(loaded);
    
    int w, h;
    w = converted->w;
    h = converted->h;
    
    // Texture
    wgpu::TextureDescriptor textureDesc = {};
    textureDesc.dimension = wgpu::TextureDimension::e2D;
    textureDesc.size = {(uint32_t)w, (uint32_t)h, 1};
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
    textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
    textureDesc.viewFormatCount = 0;
    
    // Texture View
    wgpu::TextureViewDescriptor textureViewDesc = {};
    textureViewDesc.aspect = wgpu::TextureAspect::All;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
    textureViewDesc.format = textureDesc.format;
    
    // Sampler
    wgpu::SamplerDescriptor samplerDesc = {};
    samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
    samplerDesc.magFilter = wgpu::FilterMode::Nearest;
    samplerDesc.minFilter = wgpu::FilterMode::Nearest;
    samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Nearest;
    samplerDesc.lodMinClamp = 0.0f;
    samplerDesc.lodMaxClamp = 1.0f;
    samplerDesc.compare = wgpu::CompareFunction::Undefined;
    samplerDesc.maxAnisotropy = 1;
    
    Image image = {};
    image.w = w;
    image.h = h;
    image.texture = device.CreateTexture(&textureDesc);
    image.view = image.texture.CreateView(&textureViewDesc);
    image.sampler = device.CreateSampler(&samplerDesc);
    
    wgpu::TexelCopyTextureInfo dst = {};
    dst.texture = image.texture;
    dst.mipLevel = 0;
    
    wgpu::TexelCopyBufferLayout src = {};
    src.bytesPerRow = converted->pitch;
    src.rowsPerImage = converted->h;
    
    queue.WriteTexture(
        &dst, 
        converted->pixels, 
        w * h * sizeof(uint32_t), 
        &src, 
        &textureDesc.size
    );
    
    SDL_DestroySurface(converted);
    
    images.emplace(name, image);
    return images.at(name);
}

void Resources::Init(wgpu::Device device, wgpu::Queue queue)
{
    this->device = device;
    this->queue = queue;
}

Image& Resources::GetImage(std::string name)
{
    return images.at(name);
}