#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <SFML/Graphics.hpp>

struct Config {
    Config();
    sf::Vector2u window_size;
    float L;
    float T;
    float V;
    float A;
    float health_rate;
    uint fps;
    float dt;
    float dx;
};

class CenteredCircleShape : public sf::CircleShape {
   public:
    // Constructor that sets a different default origin
    CenteredCircleShape(float radius = 50.f);
};