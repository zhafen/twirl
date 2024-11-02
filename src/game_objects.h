#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <SFML/Graphics.hpp>

class Particle : public sf::CircleShape {

public:
    Particle(sf::Vector2f r, sf::Vector2f v, float R);

    void setPosition(sf::Vector2f position);
    void applyPhysics(sf::Vector2f a, float dt);

    sf::Vector2f r;
    sf::Vector2f v;
};

#endif