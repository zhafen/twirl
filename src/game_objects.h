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

class Particle : public sf::CircleShape {

public:
    Particle(sf::Vector2f r, sf::Vector2f v, float R, const Config& cfg);

    void setPosition(sf::Vector2f position);
    void updateState(sf::Vector2f a, float dt);

    Config cfg;
    sf::Vector2f r;
    sf::Vector2f v;
    float health = 1.f;
};

class ValueBar : public sf::RectangleShape {
public:
    ValueBar(float max_value, float max_size, const Config& cfg);

private:
    Config cfg;
    float max_value;
    float max_length;
};

class Player {
public:
    Player(sf::Vector2f r, sf::Vector2f v, float R, const Config& cfg);

    void update();
    void draw(sf::RenderWindow& window);

    Particle body_particle;

private:

    Config cfg;
    sf::Vector2f r_bt;
    Particle target_particle;
    ValueBar health_bar;
};

#endif