#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <string>
#include <unordered_map>
#include <any>
#include <variant>

#include "Renderer.hpp"

struct Position { int x; int y; };
struct Tag { std::string name; };
struct Sprite { std::string sprite; };
struct Scale { int x; int y; };
struct Color { int r; int g; int b; };
struct Opacity { float a; };
struct Layer {LayerType layerType;};
struct Anchor { AnchorPosition anchorPosition; };
struct Data { std::unordered_map<std::string, std::any> values; };

struct Area { 
    glm::vec4 shape = {};
    float size = 1.0f;  
};

using Components = std::variant<Position, Tag, Scale, Color, Opacity, Layer, Anchor, Area, Data, Sprite>;

#endif