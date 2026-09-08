#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <memory>
#include <vector>
#include <webgpu/webgpu_cpp.h>
#include <unordered_map>
#include <string>

#include "glm/glm.hpp"

class Entity;

namespace Uniforms
{
    struct PVM
    {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
    };
    
    static_assert(sizeof(PVM) % 16 == 0);

    struct Material
    {
        glm::vec4 color;
        glm::vec4 uvRect;
    };
    
    static_assert(sizeof(Material) % 16 == 0);
    
};

enum class LayerType
{
  BG,
  GAME,
  FG,
  UI,
};

enum class AnchorPosition
{
    CENTER,
    BOTTOM
};

class Renderer
{
    public:
        wgpu::Device device;
        wgpu::Queue queue;
        
        
        wgpu::TextureFormat format;
        wgpu::Surface surface;
        wgpu::SurfaceTexture surfaceTexture;

        
        std::vector<LayerType> layers {LayerType::BG, LayerType::GAME, LayerType::FG, LayerType::UI};
        
        std::unordered_map<std::string, wgpu::RenderPipeline> pipelines;
        void Init(
            wgpu::Device device, 
            wgpu::Queue queue, 
            wgpu::TextureFormat format,
            wgpu::Surface surface
        );
        void ConfigureRenderer();
        
        void Render(std::vector<std::shared_ptr<Entity>>& entities, const glm::vec2& cameraPosition);
        void TextRenderer(std::shared_ptr<Entity> entity, glm::mat4 projection, glm::vec2 cameraOffset, wgpu::RenderPassEncoder pass);

        
        void CreatePipeline(
            std::string name, 
            std::string shaders, 
            wgpu::VertexBufferLayout vertexBufferLayout,
            wgpu::BindGroupLayout bindGroupLayout
        );
        
        wgpu::RenderPipeline& GetPipeline(std::string name);
        wgpu::TextureView GetCurrentTextureView();
        wgpu::BindGroupLayout bindGroupLayout;
        
};

#endif