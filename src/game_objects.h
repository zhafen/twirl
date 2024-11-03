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
    uint fps;
    float dt;
    float dx;
};

class Particle : public sf::CircleShape {

public:
    Particle(sf::Vector2f r, sf::Vector2f v, float R);

    void setPosition(sf::Vector2f position);
    void applyPhysics(sf::Vector2f a, float dt);

    sf::Vector2f r;
    sf::Vector2f v;
    float health = 1.f;
};

class Player : public Particle {
public:
    Player(sf::Vector2f r, sf::Vector2f v, float R);
};

#endif