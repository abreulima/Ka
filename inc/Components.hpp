#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <string>
#include <unordered_map>
#include <any>
#include <variant>

#include "Renderer.hpp"

struct Position { int x; int y; };
struct Tag { std::string name; };
struct Sprite { std::string name; };

struct Scale { 
    int x; int y;

    Scale(int v) : x(v), y(v) {}
    Scale(int x, int y) : x(x), y(y) {}
};

struct Color { int r; int g; int b; };
struct Opacity { float a; };
struct Layer {LayerType layerType;};
struct Anchor { AnchorPosition anchorPosition; };
struct Data { std::unordered_map<std::string, std::any> values; };
struct Area { glm::vec4 shape = {}; float size = 1.0f;  };

// Games Related

struct Platformer { 
    float moveSpeed = 120.0f;
    float jumpSpeed = 400.0f;
    float gravity = 1200.0f;
    glm::vec2 velocity = {0.0f, 0.0f};
    bool isGrounded = false;
};

using Components = std::variant<
    Position, 
    Tag, 
    Scale, 
    Color, 
    Opacity, 
    Layer, 
    Anchor, 
    Area, 
    Data, 
    Sprite,
    // Games Related
    Platformer
    >;

#endif