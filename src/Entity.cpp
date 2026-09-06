#include "Components.hpp"
#include "Resources.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <optional>
#include <type_traits>
#include <vector>

// Entity.cpp
#include "../inc/Entity.hpp"
#include "../inc/Engine.hpp"

#include <iostream>

void Entity::PopulateComponents(const std::vector<Components>& components)
{

    // Default
    // Maybe one day I will be able to get out from it
    tag = "*";
    isActive = true;
    position = glm::vec2(100, 100);
    color = glm::vec4{1.0, 1.0, 1.0, 1.0};
    layer = LayerType::GAME;
    scale = glm::vec2(1, 1);
    url = std::nullopt;
    isFlipped = false;
    anchorOffset = glm::vec2(0);
    //std::optional<glm::vec4> area = std::nullopt;
    rotation = 0.0f;

    
    //Image* line = nullptr;

    // This approach is very silly, I copy the contents of the entity
    // then I loop 
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
                this->sprite = c.name;
            }

            else if constexpr (std::is_same_v<T, AnimatedSprite>)
            {
                this->isAnimated = true;
                this->animationSize = c.size;
                this->animations = c.animations;
                this->currentAnimationName = c.currentAnimation; // CRIT
                this->sprite = c.source;
            }
            
            else if constexpr (std::is_same_v<T, Fixed>)
            {
                this->isFixed = true;
            }

            else if constexpr (std::is_same_v<T, Text>)
            {
                glyps = engine.resources.CreateTextGlyphs(
                    c.content,
                    c.font,
                    c.color
                );
            }

            else if constexpr (std::is_same_v<T, Line>)
            {
                line = this->engine.resources.CreateLine(c.start, c.end, c.color);
            }

            else if constexpr (std::is_same_v<T, Rect>)
            {
                rect = this->engine.resources.CreateRectangle(c.w, c.h, SDL_Color{255, 0, 0, 255});
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
            spriteTopleft.x + area->x * scale.x,
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

/*
void Entity::LuaOnUpdate(float dt)
{

    if (!luaOnUpdate.valid())
        return ;

    sol::protected_function_result result =
        luaOnUpdate(std::ref(*this), dt);

    if (!result.valid())
    {
        sol::error error = result;
        std::cerr << "Entity OnUpdate Error: " << error.what() << std::endl;
    }
}
*/
