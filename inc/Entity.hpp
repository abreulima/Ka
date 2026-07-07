#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Components.hpp"
#include <SDL3/SDL_rect.h>
#include <any>
#include <variant>
#include <vector>
#include <functional>
#include <optional>

#include "Renderer.hpp"
#include "Resources.hpp"
#include "glm/glm.hpp"

#include <webgpu/webgpu_cpp.h>


class Entity
{
    
    private:
    //bool isFlipped;
    
    public:
    std::string tag;
    bool isActive;
    std::string sprite;
    glm::vec2 textureSize;
    glm::vec2 scale;
    Image* image;
    LayerType layer;
    bool isFlipped;
    float rotation;
    
    std::optional<SDL_FRect> area;
    
    glm::vec2 anchorOffset;
    std::optional<std::string> url = std::nullopt;
 
    
    
    std::vector<Components> components;    
    
    std::optional<std::function<void(float)>> onUpdate = std::nullopt;
    std::optional<std::function<float(void)>> onStart = std::nullopt;
    std::optional<std::function<float(void)>> onRender = std::nullopt;
    
    // Button Related
    std::optional<std::function<void(void)>> onClick = std::nullopt;
    std::optional<std::function<void(void)>> onHover = std::nullopt;
    std::optional<std::function<void(void)>> onHoverEnd = std::nullopt;

    wgpu::RenderPipeline* pipeline = nullptr;
    wgpu::Buffer vertexBuffer;
    wgpu::Buffer indexBuffer;
    
    std::vector<wgpu::Buffer> uniformBuffers;
    
    wgpu::BindGroup bindGroup;
    
    glm::vec2 position;
    glm::vec4 color = {255, 255, 255, 255};
    
    void PopulateComponents(const std::vector<Components>& components);
    
    // Methods
    void Flip(bool state);
    SDL_FRect GetPositionRect();
    
    template <typename T>
    T* GetComponent()
    {
        for (auto& component : components)
        {
            if (auto* value = std::get_if<T>(&component))
                return value;
        }
        return nullptr;
    }
    
    template <typename T>
    T* GetData(const std::string& key)
    {
        auto* data = GetComponent<Data>();
        
        if (!data)
            return nullptr;
        
        auto it = data->values.find(key);
        
        if (it == data->values.end())
            return nullptr;
        
        return std::any_cast<T>(&it->second);
    }
    
};

#endif