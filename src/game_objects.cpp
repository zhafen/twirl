#include "game_objects.h"

#include <SFML/Graphics.hpp>

Config::Config()
    : window_size(1920u, 1080u),
      // Base scales
      L(0.01f * window_size.x),
      T(1.0f),
      V(L / T),
      A(V / T),
      health_rate(1.f / T),
      // By setting dt to not vary depending on how long it took to generate a frame,
      // our simulation slows down when it takes longer to render.
      fps(144u),
      dt(1.f / fps),
      dx(V * dt) {}

Particle::Particle(sf::Vector2f r, sf::Vector2f v, float R, const Config& cfg)
    : sf::CircleShape(R), r(r), v(v), cfg(cfg) {
    // Set the origin as the center of the particle
    setOrigin(R, R);
    setPosition(r);
}

void Particle::setPosition(sf::Vector2f position) {
    r = position;
    sf::CircleShape::setPosition(position);
}

void Particle::updateState(sf::Vector2f a, float dt) {
    // Update using leapfrog algorithm
    v += a * dt / 2.f;
    r += v * dt;
    v += a * dt / 2.f;
    setPosition(r);
}

ValueBar::ValueBar(float max_value, float max_length, const Config& cfg)
    : sf::RectangleShape(sf::Vector2f(max_length * max_value, cfg.L)),
      max_value(max_value),
      max_length(max_length),
      cfg(cfg) {
    setFillColor(sf::Color::White);
    setOutlineThickness(cfg.L / 10.f);
    setOutlineColor(sf::Color::Black);
}

Player::Player(sf::Vector2f r, sf::Vector2f v, float R, const Config& cfg)
    : body(r, v, R, cfg),
      target_particle(r, v, R / 2, cfg),
      health_bar(1.f, cfg.window_size.x / 2.f, cfg) {
        target_particle.setFillColor(sf::Color::Black);
        target_particle.setOutlineColor(sf::Color::White);
      }

void Player::draw(sf::RenderWindow& window) {
    window.draw(body);
}
