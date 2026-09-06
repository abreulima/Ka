#include "../inc/Scene.hpp"
#include <complex>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <vector>

#include "../inc/Engine.hpp"
#include "../inc/Renderer.hpp"
#include "Resources.hpp"

Entity& Scene::Add(const std::vector<Components> &components)
{
    auto entity = std::make_shared<Entity>(engine);
    entity->PopulateComponents(components);
    
    
    entity->uniformBuffers.resize(2);
    
    wgpu::BufferDescriptor pvmBufferDesc = {};
    pvmBufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
    pvmBufferDesc.mappedAtCreation = false;
    pvmBufferDesc.size = sizeof(Uniforms::PVM);
    entity->uniformBuffers[0] = engine.backend.GetDevice().CreateBuffer(&pvmBufferDesc);
    
    wgpu::BufferDescriptor materialBufferDesc = {};
    materialBufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
    materialBufferDesc.mappedAtCreation = false;
    materialBufferDesc.size = sizeof(Uniforms::Material);
    entity->uniformBuffers[1] = engine.backend.GetDevice().CreateBuffer(&materialBufferDesc);
    
    entity->pipeline = &engine.renderer.GetPipeline("default");
    
    std::vector<float> vertexData = 
    {
        // position      // uv
        0.0f, 0.0f,     0.0f, 0.0f,  // top-left
        1.0f, 0.0f,     1.0f, 0.0f,  // top-right
        1.0f, 1.0f,     1.0f, 1.0f,  // bottom-right
        0.0f, 1.0f,     0.0f, 1.0f,  // bottom-left
    };
    
    std::vector<uint16_t> indexData = 
    {
        0, 1, 2,
        0, 2, 3
    };
    
    /* Vertex and Index Buffer */
    wgpu::BufferDescriptor vertexBuffDesc = {};
    vertexBuffDesc.size = vertexData.size() * sizeof(float);
    vertexBuffDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
    vertexBuffDesc.mappedAtCreation = false;
    
    entity->vertexBuffer =  engine.backend.GetDevice().CreateBuffer(&vertexBuffDesc);
    engine.backend.GetQueue().WriteBuffer(entity->vertexBuffer, 0, vertexData.data(), vertexBuffDesc.size);
    
    wgpu::BufferDescriptor indexBuffDesc = {};
    indexBuffDesc.size = indexData.size() * sizeof(uint16_t);
    indexBuffDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
    indexBuffDesc.mappedAtCreation = false;
    
    entity->indexBuffer =  engine.backend.GetDevice().CreateBuffer(&indexBuffDesc);
    engine.backend.GetQueue().WriteBuffer(entity->indexBuffer, 0, indexData.data(), indexBuffDesc.size);
    
    /*
    wgpu::BindGroupDescriptor bindGroupDescriptor = {};
    bindGroupDescriptor.layout = engine.renderer.GetBindGroupLayout();
    bindGroupDescriptor.entryCount = entries.size();
    bindGroupDescriptor.entries = entries.data();
    */


    auto createBindGroup = [&](const Image& image, const wgpu::Buffer& pvmBuffer)
        {
            std::vector<wgpu::BindGroupEntry> entries(4);
            
            entries[0] = {};
            entries[0].binding = 0;
            entries[0].buffer = pvmBuffer;
            entries[0].offset = 0;
            entries[0].size = sizeof(Uniforms::PVM);
            
            // Attributes
            entries[1] = {};
            entries[1].binding = 1;
            entries[1].buffer = entity->uniformBuffers[1];
            entries[1].offset = 0;
            entries[1].size = sizeof(Uniforms::Material);
            
            // Texture
            entries[2] = {};
            entries[2].binding = 2;
            entries[2].textureView = image.view;
            
            // Sampler
            entries[3] = {};
            entries[3].binding = 3;
            entries[3].sampler = image.sampler;
            
            wgpu::BindGroupDescriptor bindGroupDescriptor = {};
            bindGroupDescriptor.layout = engine.renderer.bindGroupLayout;
            bindGroupDescriptor.entryCount = entries.size();
            bindGroupDescriptor.entries = entries.data();
            
            return engine.backend.GetDevice().CreateBindGroup(
                &bindGroupDescriptor
            );  
        };

    if (entity->glyps.has_value())
    {
        int textW = 0;
        int textH = 0;

        for (Glyph& glyph : entity->glyps.value())
        {
            textW += glyph.advance;
            textH = std::max(textH, glyph.h);

            glyph.pvmBuffer = engine.backend.GetDevice().CreateBuffer(
                &pvmBufferDesc
            );

            glyph.bindGroup = createBindGroup(
                          glyph.image,
                          glyph.pvmBuffer
                      );
        }

        entity->textureSize = glm::vec2(
            static_cast<float>(textW),
            static_cast<float>(textH)
        );

        entities.push_back(entity);
        return *entity;
        
    }

    Image *image = nullptr;
    
    if (entity->line)
    {
        image = &entity->line.value();
    }
    else if (entity->rect)
    {
        image = &entity->rect.value();
    }
    else if (!entity->sprite.empty())
    {
        image = &engine.resources.GetImage(entity->sprite);
    }

    if (!image)
    {
        std::cout << "Entity is not renderable!" << std::endl;
        exit(1);
    }

    if (entity->isAnimated)
    {
        glm::vec2 textureSize = glm::vec2(image->w, image->h);
        entity->spriteSize = glm::vec2(image->w, image->h);
        
        // Convert pixels to [0, 1]
        entity->normalizedFrameSize = glm::vec2(
            entity->animationSize / textureSize
        );
        
        entity->textureSize = entity->animationSize;
    }
    else
    {
        entity->textureSize = glm::vec2(image->w, image->h); 
    }
    
        
    entity->bindGroup = createBindGroup(*image, entity->uniformBuffers[0]);
    
    entities.push_back(entity);
    return *entity;
}
