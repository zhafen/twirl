#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>

class Particle2 : public sf::CircleShape {

public:

    Particle2(sf::Vector2f r, sf::Vector2f v, float R);

    sf::Vector2f r;
    sf::Vector2f v;

    // void update(sf::Vector2f a, float dt);
    // void setPosition(sf::Vector2f pos);
};

#endif