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
    : bar(sf::Vector2f(max_length * max_value, cfg.L)),
      value(max_value),
      max_value(max_value),
      max_length(max_length),
      cfg(cfg) {
    bar.setFillColor(sf::Color::White);
    bar.setOutlineThickness(cfg.L / 10.f);
    bar.setOutlineColor(sf::Color::Black);
}

void ValueBar::draw(sf::RenderWindow& window, sf::View& view) {

    // Resize
    float length = value / max_value * max_length;
    bar.setSize(sf::Vector2f(length, cfg.L));
    sf::Vector2i pixel_pos(cfg.window_size.x / 2.f - length / 2.f, cfg.L);
    bar.setPosition(window.mapPixelToCoords(pixel_pos));

    window.draw(bar);
}

Player::Player(sf::Vector2f r, sf::Vector2f v, float R, const Config& cfg)
    : cfg(cfg),
      body_particle(r, v, R, cfg),
      target_particle(r, v, R / 2, cfg),
      health_bar(1.f, cfg.window_size.x / 2.f, cfg),
      r_bt(sf::Vector2f(0.f, 0.f)) {
        target_particle.setFillColor(sf::Color::Black);
        target_particle.setOutlineThickness(cfg.L / 10.f);
        target_particle.setOutlineColor(sf::Color::White);
      }

void Player::updateState(bool is_colliding) {

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) {
        r_bt += sf::Vector2f(cfg.dx, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left)) {
        r_bt += sf::Vector2f(-cfg.dx, 0.f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) {
        r_bt += sf::Vector2f(0.f, -cfg.dx);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down)) {
        r_bt += sf::Vector2f(0.f, cfg.dx);
    }

    // Directional force
    float r2 = powf(r_bt.x, 2.f) + powf(r_bt.y, 2.f);
    sf::Vector2f a =
        5.f * cfg.A * (r_bt / powf(r2 + powf(cfg.L, 2.f), 1.5f)) * cfg.L * cfg.L;

    // Update the player's body
    body_particle.updateState(a, cfg.dt);
    target_particle.setPosition(body_particle.r + r_bt);

    // Update the player's health
    if (is_colliding) {
        health_bar.value -= cfg.health_rate * cfg.dt;
    }
}

void Player::draw(sf::RenderWindow& window, sf::View& view) {
    window.draw(body_particle);
    window.draw(target_particle);
    health_bar.draw(window, view);
}
