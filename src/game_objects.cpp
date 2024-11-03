#include "game_objects.h"

#include <SFML/Graphics.hpp>

Config::Config()
    : window_size(1920u, 1080u),
      // Base scales
      L(0.01f * window_size.x),
      T(1.0f),
      V(L / T),
      A(V / T),
      // By setting dt to not vary depending on how long it took to generate a frame,
      // our simulation slows down when it takes longer to render.
      fps(144u),
      dt(1.f / fps),
      dx(V * dt) {}

Particle::Particle(sf::Vector2f r, sf::Vector2f v, float R)
    : sf::CircleShape(R), r(r), v(v) {
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

Player::Player(sf::Vector2f r, sf::Vector2f v, float R) : Particle(r, v, R) {}
