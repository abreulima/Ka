#include "../inc/Renderer.hpp"

#include <cassert>
#include <cstddef>
#include <SDL3/SDL.h>

#include "../inc/Backend.hpp"

#include "../inc/glm/gtc/matrix_transform.hpp"

#include "../inc/Entity.hpp"
#include <cstdint>
#include <iostream>

void Renderer::CreatePipeline(
    std::string name,
    std::string shaderPath, 
    wgpu::VertexBufferLayout vertexBufferLayouts,
    //std::vector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntries
    wgpu::BindGroupLayout bindGroupLayout
)
{
    void *shaderFile = SDL_LoadFile(shaderPath.c_str(), NULL);
    if (!shaderFile)
    {
        std::cout << SDL_GetError() << std::endl;
        return ;
    }
    
    wgpu::ShaderSourceWGSL shaderSource = {};
    shaderSource.sType = wgpu::SType::ShaderSourceWGSL;
    shaderSource.code = static_cast<char *>(shaderFile);
    
    wgpu::ShaderModuleDescriptor shaderModuleDescrpitor = {};
    shaderModuleDescrpitor.nextInChain = &shaderSource;
    
    wgpu::ShaderModule shaderModule = {};
    shaderModule = this->device.CreateShaderModule(&shaderModuleDescrpitor);
    if (!shaderModule)
    {
        std::cerr << "Unable to create shader module." << std::endl;
        return ;
    }
    
    wgpu::VertexState vertexState = {};
    vertexState.module = shaderModule;
    vertexState.entryPoint = "vs_main";
    vertexState.bufferCount = 1;
    vertexState.buffers = &vertexBufferLayouts; // IMPORTANT
    
    wgpu::BlendState blendState = {};
    blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;
    blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
    blendState.alpha.dstFactor = wgpu::BlendFactor::One;
    blendState.alpha.operation = wgpu::BlendOperation::Add;
    
    wgpu::ColorTargetState colorTargetState = {};
    colorTargetState.format = this->format;
    colorTargetState.blend = &blendState;
    colorTargetState.writeMask = wgpu::ColorWriteMask::All;
    
    wgpu::FragmentState fragmentState = {};
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTargetState;
    
    wgpu::PrimitiveState primititiveState = {};
    primititiveState.topology = wgpu::PrimitiveTopology::TriangleList;
    primititiveState.stripIndexFormat = wgpu::IndexFormat::Undefined;
    primititiveState.frontFace = wgpu::FrontFace::CCW;
    primititiveState.cullMode = wgpu::CullMode::None;
    
    wgpu::MultisampleState multisampleState = {};
    multisampleState.count = 1;
    multisampleState.mask = ~0u;
    multisampleState.alphaToCoverageEnabled = false;
    
    wgpu::PipelineLayoutDescriptor pipelineLayoutDescriptor = {};
    pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
    pipelineLayoutDescriptor.bindGroupLayouts = &bindGroupLayout;
    wgpu::PipelineLayout pipelineLayout = device.CreatePipelineLayout(&pipelineLayoutDescriptor);
   
   wgpu::RenderPipelineDescriptor renderPipelineDescriptor = {};
   renderPipelineDescriptor.layout = pipelineLayout;
   renderPipelineDescriptor.vertex = vertexState;
   renderPipelineDescriptor.fragment = &fragmentState;
   renderPipelineDescriptor.primitive = primititiveState;
   renderPipelineDescriptor.multisample = multisampleState;
   renderPipelineDescriptor.depthStencil = nullptr;
   
   pipelines[name] = device.CreateRenderPipeline(&renderPipelineDescriptor);
   
   SDL_free(shaderFile);
   
   
   
}

void Renderer::ConfigureRenderer()
{
    // Create basic pipeline for general use 2D 

    std::vector<wgpu::VertexAttribute> vAttributes(2);
    
    // Position
    vAttributes[0] = {};
    vAttributes[0].shaderLocation = 0;
    vAttributes[0].format = wgpu::VertexFormat::Float32x2;
    vAttributes[0].offset = 0;
    
    // UV
    vAttributes[1].shaderLocation = 1;
    vAttributes[1].format = wgpu::VertexFormat::Float32x2;
    vAttributes[1].offset = 2 * sizeof(float);
    
    wgpu::VertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.attributeCount = vAttributes.size();
    vertexBufferLayout.attributes = vAttributes.data();
    vertexBufferLayout.arrayStride = 4 * sizeof(float);
    vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;
    
    std::vector<wgpu::BindGroupLayoutEntry> entries(4);
    
    // PVM Uniforms
    entries[0] = {};
    entries[0].binding = 0;
    entries[0].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
    entries[0].buffer.type = wgpu::BufferBindingType::Uniform;
    entries[0].buffer.minBindingSize = sizeof(Uniforms::PVM);

    // Material    
    entries[1] = {};
    entries[1].binding = 1;
    entries[1].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
    entries[1].buffer.type = wgpu::BufferBindingType::Uniform;
    entries[1].buffer.minBindingSize = sizeof(Uniforms::Material);
    
    // Texture 
    entries[2] = {};
    entries[2].binding = 2;
    entries[2].visibility = wgpu::ShaderStage::Fragment;
    entries[2].texture.sampleType = wgpu::TextureSampleType::Float;
    entries[2].texture.viewDimension = wgpu::TextureViewDimension::e2D;
    
    // Sampler
    entries[3] ={};
    entries[3].binding = 3;
    entries[3].visibility = wgpu::ShaderStage::Fragment;
    entries[3].sampler.type = wgpu::SamplerBindingType::Filtering;
    
    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
    bindGroupLayoutDescriptor.entryCount = entries.size();
    bindGroupLayoutDescriptor.entries = entries.data();
    bindGroupLayout = device.CreateBindGroupLayout(&bindGroupLayoutDescriptor);
    
    
    CreatePipeline(
    "default", 
    "res/shaders/default.wgsl", 
    vertexBufferLayout,
    bindGroupLayout
    );
    
    
}

wgpu::RenderPipeline& Renderer::GetPipeline(std::string name)
{
    return pipelines.at(name);
}

void Renderer::Init(
    wgpu::Device device, 
    wgpu::Queue queue, 
    wgpu::TextureFormat format,
    wgpu::Surface surface
)
{
    this->device = device;
    this->queue = queue;
    this->format = format;
    this->surface = surface;
    //layers.
    ConfigureRenderer();
}

void Renderer::Render(std::vector<std::shared_ptr<Entity>> entities)
{
    wgpu::TextureView targetView = GetCurrentTextureView();
    if (!targetView)
        return ;
    
    wgpu::CommandEncoder cmdEnconder = device.CreateCommandEncoder();
    wgpu::RenderPassEncoder pass;
    
    // First Pass
    wgpu::RenderPassColorAttachment colorAttch = {};
    colorAttch.view = targetView;
    colorAttch.loadOp = wgpu::LoadOp::Clear;
    colorAttch.storeOp = wgpu::StoreOp::Store;
    colorAttch.clearValue = {100 / 255.0, 149 / 255.0, 237 / 255.0, 1.0f};
    
    wgpu::RenderPassDescriptor renderPassDesc = {};
    renderPassDesc.nextInChain = nullptr;
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &colorAttch;
    renderPassDesc.depthStencilAttachment = nullptr;
    renderPassDesc.timestampWrites = nullptr;
    pass = cmdEnconder.BeginRenderPass(&renderPassDesc);
    
    // Camera 
    glm::mat4 projection = glm::ortho(0.0f, float(WIDTH), float(HEIGHT), 0.0f, -1.0f, 1.0f);
    
    for (auto layer : layers)
    {
        for (auto &entity : entities)
        {
            if (!entity->isActive) continue;
            if (entity->layer != layer) continue;
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(entity->position, 0.0f));
            model = glm::scale(model, glm::vec3(entity->textureSize * entity->scale, 1.0f));
            model = glm::translate(model, glm::vec3(-entity->anchorOffset, 0.0f));
            
            Uniforms::PVM pvm;
            pvm.projection = projection;
            pvm.view = glm::mat4(1.0f);
            pvm.model = model;
            
            Uniforms::Material material;
            material.color = entity->color;
            
            material.uvRect = entity->isFlipped ? 
                    glm::vec4(1.0f, 0.0f, -1.0f, 1.0f) : 
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            
            assert(entity->pipeline != nullptr);
            pass.SetPipeline(*entity->pipeline);
                       
            queue.WriteBuffer(entity->uniformBuffers[0], 0, &pvm, sizeof(Uniforms::PVM));
            queue.WriteBuffer(entity->uniformBuffers[1], 0, &material, sizeof(Uniforms::Material));
            
            pass.SetBindGroup(0, entity->bindGroup, 0, nullptr);
            pass.SetVertexBuffer(0, entity->vertexBuffer, 0, entity->vertexBuffer.GetSize());
            pass.SetIndexBuffer(entity->indexBuffer, wgpu::IndexFormat::Uint16, 0, entity->indexBuffer.GetSize());
            
            pass.DrawIndexed(entity->indexBuffer.GetSize() / sizeof(uint16_t), 1, 0, 0);        
        }
    }
        
    pass.End();

    wgpu::CommandBuffer cmdBuffer = cmdEnconder.Finish();
    queue.Submit(1, &cmdBuffer);
    
    #ifndef __EMSCRIPTEN__
    surface.Present();
    #endif
    
}

wgpu::TextureView Renderer::GetCurrentTextureView()
{
    wgpu::TextureView textureView = nullptr;
    
    surface.GetCurrentTexture(&surfaceTexture);
    
    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
        surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal)
        return nullptr;
    
    wgpu::TextureViewDescriptor textureViewDesc = {};
    textureViewDesc.nextInChain = nullptr;
    textureViewDesc.format = surfaceTexture.texture.GetFormat();
    textureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.aspect = wgpu::TextureAspect::All;
    
    textureView = surfaceTexture.texture.CreateView(&textureViewDesc);
    
    if (!textureView)
        return nullptr;
    
    return textureView;
}