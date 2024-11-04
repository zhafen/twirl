#include "game.h"

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "game_objects.h"
#include "systems.h"

Game::Game()
    : cfg(),
      window(sf::VideoMode(cfg.window_size.x, cfg.window_size.y), "twirl"),
      view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size)),
      p(sf::Vector2f(0.f, 20.f * cfg.L), sf::Vector2f(0.f, -cfg.V), cfg.L, cfg),
      render_system(cfg, view) {
    window.setFramerateLimit(cfg.fps);
    initializeState();
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::resetGameState() {
    // Reset all systems
    // renderSystem.reset();

    // Reinitialize the game state
    initializeState();
}

int Game::createEntity() { return entityCounter++; }

void Game::initializeState() {
    // Make player
    EntityId id = createEntity();
    RenderComponent rc;
    rc.shape = sf::CircleShape(cfg.L);
    components.render_comps[id] = rc;
    PhysicsComponent pc;
    pc.pos = sf::Vector2f(0.f, 20.f * cfg.L);
    pc.vel = sf::Vector2f(0.f, -cfg.V);
    components.physics_comps[id] = pc;

    // Make enemies
    std::random_device rd;
    std::mt19937 gen(rd());  // Standard random number generator
    std::uniform_real_distribution<float> dist(-10.f * cfg.L, 10.f * cfg.L);
    int n_enemies = 10;
    for (int i = 0; i < n_enemies; ++i) {
        EntityId id = createEntity();
        RenderComponent rc;
        rc.shape = sf::CircleShape(cfg.L);
        rc.shape.setFillColor(sf::Color::Red);
        components.render_comps[id] = rc;
        PhysicsComponent pc;
        pc.pos = sf::Vector2f(dist(gen), dist(gen));
        pc.vel = sf::Vector2f(0.f, 0.f);
        components.physics_comps[id] = pc;
    }

    // Make background
    int n_bkgrd = 100;
    for (int i = 0; i < n_bkgrd; ++i) {
        EntityId id = createEntity();
        RenderComponent rc;
        rc.shape = sf::CircleShape(10.f * cfg.L * i);
        rc.shape.setFillColor(sf::Color(127, 127, 127));
        rc.shape.setOutlineThickness(cfg.L / 5.f);
        rc.shape.setOutlineColor(sf::Color(63, 63, 63));
        rc.shape.setOrigin(rc.shape.getRadius(), rc.shape.getRadius());
        rc.zorder = -i;
        components.render_comps[id] = rc;
        PhysicsComponent pc;
        pc.pos = sf::Vector2f(0.f, 0.f);
        pc.vel = sf::Vector2f(0.f, 0.f);
        components.physics_comps[id] = pc;
    }

    // Create a vector of (zorder, id) pairs
    for (const auto& [id, rc] : components.render_comps) {
        components.entity_zorders.emplace_back(rc.zorder, id);
    }

    // Sort the vector according to zorder
    std::sort(components.entity_zorders.begin(), components.entity_zorders.end());
}

void Game::handleEvents() {
    for (auto event = sf::Event(); window.pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void Game::update() {
    // Collision detection
    bool is_colliding = false;
    for (int i = 0; i < enemies.size(); ++i) {
        bool is_colliding_i =
            p.body_particle.getGlobalBounds().intersects(enemies[i].getGlobalBounds());
        is_colliding = is_colliding | is_colliding_i;
    }
    p.updateState(is_colliding);

    // Update game state
    for (int i = 0; i < enemies.size(); ++i) {
        sf::Vector2f r_et = p.body_particle.r - enemies[i].getPosition();
        float r2 = powf(r_et.x, 2.f) + powf(r_et.y, 2.f);
        sf::Vector2f a =
            5.f * cfg.A * (r_et / powf(r2 + powf(cfg.L, 2.f), 1.5f)) * cfg.L * cfg.L;
        enemies[i].updateState(a, cfg.dt);
    }
}

void Game::render() {
    // Set the view
    view.setCenter(p.body_particle.r);
    window.setView(view);

    render_system.render(window, components);
}