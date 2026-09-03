#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <webgpu/webgpu_cpp.h>
#include <glm/glm.hpp>

struct Image
{
    wgpu::Texture texture;
    wgpu::TextureView view;
    wgpu::Sampler sampler;
    uint32_t w;
    uint32_t h;
};

struct Font {
    TTF_Font* font;
    uint8_t size;
};

struct Glyph {
    Image image;

    int h;
    int w;

    int minx;
    int maxx;
    int miny;
    int maxy;

    int advance;

    wgpu::Buffer pvmBuffer;
    wgpu::BindGroup bindGroup;

};


class Resources
{
    private:
        wgpu::Device device;
        wgpu::Queue queue;
        std::unordered_map<std::string, Image> images;
        std::unordered_map<std::string, Font> fonts;

        Image CreateImageFromSDLsurface(SDL_Surface& surface);
        Image CreateImage(std::vector<uint8_t> pixels, int w, int h);

    public:
        void Init(wgpu::Device, wgpu::Queue queue);

        Image LoadImage(std::string name, std::string path);
        Image& GetImage(std::string name);

        Image CreateRectFromImage(const Image& image, std::string name, glm::ivec4 rect);

        Image CreateLine(glm::vec2 start, glm::vec2 end, SDL_Color color);
        Image CreateRectangle(int w, int h, SDL_Color color);

        //
        Font LoadFont(std::string name, std::string path, uint8_t size);
        Font& GetFont(std::string name);

        std::vector<Glyph> CreateTextGlyphs(const std::string& content, const std::string& fontName, SDL_Color color);


};

#endif
