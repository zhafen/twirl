#include <SFML/Graphics.hpp>
#include "game_objects.h"

Config::Config() {
    sf::Vector2u window_size(1920u, 1080u);
    // Base scales
    float L = 0.01f * window_size.x; // Length scale
    float T = 1.0f; // Time scale (in seconds)
    float V = L / T; // Velocity scale
    float A = V / T; // Acceleration scale

    // By setting dt to not vary depending on how long it took to generate a frame,
    // our simulation slows down when it takes longer to render.
    float fps = 144;
    float dt = 1.f / fps; // Time step
    float dx = V * dt; // Displacement step
}

Particle::Particle(sf::Vector2f r, sf::Vector2f v, float R) : sf::CircleShape(R), r(r), v(v) {
    // Set the origin as the center of the particle
    setOrigin(R, R);
    setPosition(r);
}

void Particle::setPosition(sf::Vector2f position) {
    r = position;
    sf::CircleShape::setPosition(position);
}

void Particle::applyPhysics(sf::Vector2f a, float dt) {
    // Update using leapfrog algorithm
    v += a * dt / 2.f;
    r += v * dt;
    v += a * dt / 2.f;
    setPosition(r);
}

Player::Player(sf::Vector2f r, sf::Vector2f v, float R) : Particle(r, v, R) {
}
