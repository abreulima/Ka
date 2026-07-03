#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <webgpu/webgpu_cpp.h>

struct Image
{
    wgpu::Texture texture;
    wgpu::TextureView view;
    wgpu::Sampler sampler;
    uint32_t w;
    uint32_t h;
};

class Resources
{
    private:
        wgpu::Device device;
        wgpu::Queue queue;
        std::unordered_map<std::string, Image> images;
    public:
        void Init(wgpu::Device, wgpu::Queue queue);
        Image LoadImage(std::string name, std::string path);
        Image& GetImage(std::string name);
};

#endif