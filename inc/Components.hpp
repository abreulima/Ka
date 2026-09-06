#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <SDL3/SDL_pixels.h>
#include <string>
#include <unordered_map>
#include <any>
#include <variant>
#include <vector>

#include "Renderer.hpp"
#include "glm/fwd.hpp"

struct Position { 
    float x; float y; 

    Position (float x, float y) : x(x), y(y) {}
    Position (glm::vec2 pos) : x(pos.x), y(pos.y) {}
    
};



struct Tag { std::string name; };

struct Sprite { std::string name; };

struct Animation {
    int startY = 0;
    float speed = 0;;
    int currentFrame = 0;
    int numFrames = 0;
};

struct AnimatedSprite {
    std::string source;
    glm::vec2 size;
    std::string currentAnimation;
    std::unordered_map<std::string, Animation> animations;
};

struct Fixed {};

struct Scale { 
    int x; int y;

    Scale(int v) : x(v), y(v) {}
    Scale(int x, int y) : x(x), y(y) {}
};


struct Rect
{
    int w;
    int h;
};


struct Line { 

    glm::vec2 start;
    glm::vec2 end;
    SDL_Color color;

    Line(glm::vec2 start, glm::vec2 end) : start(start), end(end), color(SDL_Color{255, 0, 0, 255}) {}
    Line(glm::vec2 start, glm::vec2 end, SDL_Color color) : start(start), end(end), color(color) {}
};

struct Color { int r; int g; int b; };
struct Opacity { float a; };
struct Layer {LayerType layerType;};
struct Anchor { AnchorPosition anchorPosition; };
struct Data { std::unordered_map<std::string, std::any> values; };
struct Area { glm::vec4 shape = {}; float size = 1.0f;  };

struct Text {
    std::string content;
    std::string font = "monogram";
    SDL_Color color = SDL_Color{0, 0, 0, 255};

    //
    Text(std::string content) : content(content), font("monogram"), color(SDL_Color{0, 0, 0, 255}) {};
    Text(std::string content, std::string font) : content(content), font(font) {};    
};

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
    Line,
    Sprite,
    AnimatedSprite,
    Fixed,
    Text,
    Rect,
    // Games Related
    Platformer
    >;

#endif