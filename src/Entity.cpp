#include "../inc/Entity.hpp"
#include <SDL3/SDL_rect.h>
#include <optional>
#include <type_traits>
#include <vector>

#include <iostream>

void Entity::PopulateComponents(const std::vector<Components>& components)
{
    
    // Default
    tag = "*";
    isActive = true;
    position = glm::vec2(100, 100);
    color = glm::vec4{1.0, 1.0, 1.0, 1.0};
    layer = LayerType::GAME;
    scale = glm::vec2(1, 1);
    url = std::nullopt;
    isFlipped = false;
    anchorOffset = glm::vec2(0);
    std::optional<glm::vec4> area = std::nullopt;
    rotation = 0.0f;
    
    this->components = components;
    
    for (const auto& component : components)
    {
        std::visit([this](const auto& c) 
        {
        
            // Whatheck is this syntax???
            using T = std::decay_t<decltype(c)>;
            
            if constexpr (std::is_same_v<T, Position>)
            {
                this->position = glm::vec2{c.x, c.y};
            }
            
            else if constexpr (std::is_same_v<T, Tag>)
            {
                this->tag = c.name;
            }
            
            else if constexpr (std::is_same_v<T, Sprite>)
            {
                this->sprite = c.sprite;
            }
            
            else if constexpr (std::is_same_v<T, Scale>)
            {
                this->scale = glm::vec2(c.x, c.y);
            }
            
            else if constexpr (std::is_same_v<T, Color>)
            {
                this->color = glm::vec4(
                    c.r / 255.0,
                    c.g / 255.0,
                    c.b / 255.0,
                    color.a
                );
            }
            
            else if constexpr (std::is_same_v<T, Layer>)
            {
                this->layer = c.layerType;
            }
            
            else if constexpr (std::is_same_v<T, Anchor>)
            {
                switch (c.anchorPosition) {
                    case AnchorPosition::CENTER:
                        this->anchorOffset = glm::vec2(0.5f);
                        break ;
                    
                    case AnchorPosition::BOTTOM:
                        this->anchorOffset = glm::vec2(0.5f, 1.0);
                        break ;
                    
                    default:
                        this->anchorOffset = glm::vec2(0);
                        break ;
                }
            }

            else if constexpr (std::is_same_v<T, Area>)
            {
                
                float x = c.shape.x;
                float y = c.shape.y;
                
                float w = c.shape.z;
                float h = c.shape.w;
                
                if (w == 0 && h == 0)
                {
                    w = textureSize.x;
                    h = textureSize.y;
                };
                
                this->area = SDL_FRect{
                    .x = position.x + x,
                    .y = position.y + y,
                    .w = w * scale.x,
                    .h = h * scale.y
                };
                
                std::cout << this->tag << std::endl;
                std::cout << "x: " << this->area->x << " " << std::endl;
                std::cout << "y: " <<this->area->y << " " << std::endl;
                std::cout << "w: " << this->area->w << " " << std::endl;
                std::cout << "h: " << this->area->h << " " << std::endl;
            }
            
            
        }, component);
    }
}

void Entity::Flip(bool state)
{
    isFlipped = state;
}

SDL_FRect Entity::GetPositionRect()
{
    
    glm::vec2 spriteSize = textureSize * scale;
    glm::vec2 spriteTopleft = position - (anchorOffset * spriteSize);

    // Collision Area isn't the same as sprite area
    if (this->area)
    {
        return SDL_FRect{
            spriteTopleft.x + area->x * scale.y,
            spriteTopleft.y + area->y * scale.y,
            area->w * scale.x,
            area->h * scale.y
        };
    }
    
    return SDL_FRect{
        spriteTopleft.x,
        spriteTopleft.y,
        spriteSize.x,
        spriteSize.y
    };
}