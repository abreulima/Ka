#include "../inc/Resources.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_int4.hpp"
#include "glm/fwd.hpp"
#include "webgpu/webgpu_cpp.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>


glm::vec2 GetTextBounds(const std::vector<Glyph> &glyphs)
{
    float w = 0.0f;
    float h = 0.0f;

    for (const auto& glyph : glyphs)
    {
        w += static_cast<float>(glyph.advance);
        h = std::max(h, static_cast<float>(glyph.h));
    }

    return {w, h};
}



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
    textureDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
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
        converted->pitch *  converted->h, //w * h * sizeof(uint32_t),
        &src,
        &textureDesc.size
    );

    SDL_DestroySurface(converted);

    images.emplace(name, image);
    return images.at(name);
}

Image Resources::CreateLine(glm::vec2 start, glm::vec2 end, SDL_Color color)
{

    int h = end.y - start.y;
    int w = end.x - start.x;

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
    src.bytesPerRow = 4 * w;
    src.rowsPerImage = h;

    std::vector<uint8_t> pixels (4 * w * h, 0);

    int dx =  abs(end.x - start.x);
    int sx = start.x < end.x ? 1 : -1;

    int dy = -abs(end.y - start.y);
    int sy = start.y < end.y ? 1 : -1;

    int err = dx + dy; /* error value e_xy */

    // Bresenham
    while (true)
    {  /* loop */

        uint8_t *p = &pixels[4 * (start.y * w + start.x)];
        p[0] = color.r;
        p[1] = color.g;
        p[2] = color.b;
        p[3] = color.a;


        if (start.x == end.x && start.y == end.y) break;
        int e2 = 2 * err;

        if (e2 >= dy) { err += dy; start.x += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; start.y += sy; } /* e_xy+e_y < 0 */
    }

    queue.WriteTexture(
        &dst,
        pixels.data(),
        4 * w * h, //w * h * sizeof(uint32_t),
        &src,
        &textureDesc.size
    );

    return image;
}

Image Resources::CreateRectFromImage(const Image& image, std::string name, glm::ivec4 rect)
{
    uint32_t x = rect.x;
    uint32_t y = rect.y;
    uint32_t width = rect.z;
    uint32_t height = rect.w;

    wgpu::TextureDescriptor textureDesc = {};
    textureDesc.label = name.c_str();
    textureDesc.dimension = wgpu::TextureDimension::e2D;
    textureDesc.size = {width, height, 1};     // Size of dest.
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = wgpu::TextureFormat::RGBA8Unorm;
    textureDesc.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopySrc;

    wgpu::Texture textureDestination = device.CreateTexture(&textureDesc);
    wgpu::CommandEncoder encoder = device.CreateCommandEncoder();


    wgpu::TexelCopyTextureInfo source = {
        .texture = image.texture,
        .mipLevel = 0,
        .origin = wgpu::Origin3D{x, y, 0},
        .aspect = wgpu::TextureAspect::All,
    };

    wgpu::TexelCopyTextureInfo destination = {
        .texture = textureDestination,
        .mipLevel = 0,
        .origin = wgpu::Origin3D{0, 0, 0},
        .aspect = wgpu::TextureAspect::All,
    };

    wgpu::Extent3D extent3D = {.width = width, .height = height, .depthOrArrayLayers = 1};

    encoder.CopyTextureToTexture(&source, &destination, &extent3D);
    wgpu::CommandBuffer commandBuffer = encoder.Finish();
    queue.Submit(1, &commandBuffer);

    // Texture View
    wgpu::TextureViewDescriptor textureViewDesc = {};
    textureViewDesc.aspect = wgpu::TextureAspect::All;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
    textureViewDesc.format = wgpu::TextureFormat::RGBA8Unorm;

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


    Image out;
    out.texture = textureDestination;
    out.view = out.texture.CreateView(&textureViewDesc);
    out.sampler = device.CreateSampler(&samplerDesc);
    out.w = width;
    out.h = height;

    images.emplace(name, out);
    return images.at(name);

}

Image Resources::CreateRectangle(int w, int h, SDL_Color color)
{

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
    src.bytesPerRow = 4 * w;
    src.rowsPerImage = h;

    std::vector<uint8_t> pixels(4 * w * h, 0);

    for (int x = 0; x < w; x++)
    {
        uint8_t *pixel = &pixels[x * 4];

        pixel[0] = color.r;
        pixel[1] = color.g;
        pixel[2] = color.b;
        pixel[3] = color.a;
    }

    queue.WriteTexture(
        &dst,
        pixels.data(),
        4 * w * h, //w * h * sizeof(uint32_t),
        &src,
        &textureDesc.size
    );

    return image;
}


Font Resources::LoadFont(std::string name, std::string path, uint8_t size)
{

    Font font;
    font.font = TTF_OpenFont(path.c_str(), size);

    if (!font.font)
    {
        std::cerr << "Failed to load font: " << SDL_GetError() << std::endl;
        return {};
    }

    fonts.emplace(name, font);
    return fonts.at(name);

};

std::vector<Glyph> Resources::CreateTextGlyphs(const std::string& content, const std::string& fontname, SDL_Color color)
{

    std::vector<Glyph> glyphs;
    glyphs.reserve(content.size());

    Font& font = fonts.at(fontname);

    for (unsigned char c : content)
    {
        //std::string character(1, c);

        int minx, maxx;
        int miny, maxy;
        int advance;

        if (!TTF_GetGlyphMetrics(
            font.font,
            static_cast<uint32_t>(c),
            &minx,
            &maxx,
            &miny,
            &maxy,
            &advance
        ))
        {
            std::cerr << "Failed to Get Glyph Metrics: " << SDL_GetError() << std::endl;
            continue ;
        }

        SDL_Surface* surface = TTF_RenderGlyph_Blended(
            font.font,
            static_cast<uint32_t>(c),
            color
        );

        if (!surface)
            continue;

        SDL_Surface *converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);

        if (!converted)
        {
            SDL_DestroySurface(surface);
            continue ;
        }


        Glyph glyph{};
        glyph.image = CreateImageFromSDLsurface(*converted);
        glyph.w = converted->w;
        glyph.h = converted->h;
        glyph.advance = advance;

        glyphs.emplace_back(std::move(glyph));

        SDL_DestroySurface(converted);
        SDL_DestroySurface(surface);
    }

    return glyphs;
}


// Tidely packed convertion
// SDL_Surface* surface = ...;

/*
auto* pixels = static_cast<uint8_t*>(surface->pixels);

std::vector<uint8_t> data(
    pixels,
    pixels + surface->pitch * surface->h
);
*/

Image Resources::CreateImageFromSDLsurface(SDL_Surface& surface)
{

    int bytesPerPixel = 4;
    int rowSize = surface.w * bytesPerPixel;

    std::vector<uint8_t> data (rowSize * surface.h);

    auto* src = static_cast<uint8_t*>(surface.pixels);

    for (int y = 0; y < surface.h; ++y)
    {
        std::memcpy(
            data.data() + y * rowSize,
            src + y * surface.pitch,
            rowSize
        );
    }

    return  CreateImage(
        std::move(data),
        surface.w,
        surface.h
    );

}


Image Resources::CreateImage(std::vector<uint8_t> pixels, int w, int h)
{
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
    src.bytesPerRow = 4 * w;
    src.rowsPerImage =h;

    queue.WriteTexture(
        &dst,
        pixels.data(),
        pixels.size(), //w * h * sizeof(uint32_t),
        &src,
        &textureDesc.size
    );

    return image;
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
