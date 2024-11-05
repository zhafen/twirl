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
      render_system(cfg, view),
      physics_system(cfg) {
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
    EntityId player_id = createEntity();
    RenderComponent rc;
    rc.shape = sf::CircleShape(cfg.L);
    components.render_comps[player_id] = rc;
    PhysicsComponent pc;
    pc.pos = sf::Vector2f(0.f, 0.f);
    pc.vel = sf::Vector2f(0.f, 0.f);
    components.physics_comps[player_id] = pc;
    // Pin the view to the player
    view.setCenter(pc.pos);
    window.setView(view);

    // Make enemies
    std::random_device rd;
    std::mt19937 gen(rd());  // Standard random number generator
    std::uniform_real_distribution<float> dist(-10.f * cfg.L, 10.f * cfg.L);
    int n_enemies = 10;
    for (int i = 0; i < n_enemies; ++i) {
        EntityId id = createEntity();
        // Colored circles
        RenderComponent rc;
        rc.shape = sf::CircleShape(cfg.L);
        rc.shape.setFillColor(sf::Color::Red);
        components.render_comps[id] = rc;
        // Randomly distributed in a square
        PhysicsComponent pc;
        pc.pos = sf::Vector2f(dist(gen), dist(gen));
        pc.vel = sf::Vector2f(0.f, 0.f);
        components.physics_comps[id] = pc;
        // Pulled towards the player
        PairwiseForceComponent pfc;
        pfc.target_entity = id;
        pfc.source_entity = player_id;
        // Because of the r^-2 force drops off quickly if we don't scale it strongly
        pfc.params.magnitude *= -100.f * cfg.A;
        pfc.params.power = -2.f;
        pfc.params.softening = cfg.L;
        components.pairforce_comps[id] = pfc;
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

    // Calculate forces
    physics_system.calculateForces(components);

    // Update state
    physics_system.update(components);

    // // Collision detection
    // bool is_colliding = false;
    // for (int i = 0; i < enemies.size(); ++i) {
    //     bool is_colliding_i =
    //         p.body_particle.getGlobalBounds().intersects(enemies[i].getGlobalBounds());
    //     is_colliding = is_colliding | is_colliding_i;
    // }
    // p.updateState(is_colliding);
}

void Game::render() {
    // Set the view

    render_system.render(window, components);
}