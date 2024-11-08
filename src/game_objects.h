#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <SFML/Graphics.hpp>

// OPTIMIZE: Consider computing these at compile time since they won't change.
struct Config {
    Config();
    sf::Vector2u window_size;
    float L;
    float T;
    float V;
    float A;
    uint16_t fps;
    float dt;
    float dx;
};

class CCCircleShape : public sf::CircleShape {
   public:
    // Constructor that sets a different default origin
    CCCircleShape(float radius = 50.f);
};

#endif  // GAMEOBJECTS_H