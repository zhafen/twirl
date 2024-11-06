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
// Same as createEntity, but for relationships between entities
int Game::createEntityRelationship() { return entityRelationshipCounter++; }

void Game::initializeState() {
    // Make player
    player_id = createEntity();
    MetadataComponent mc;
    mc.id = player_id;
    mc.name = "Player";
    components.metadata_comps[player_id] = mc;
    RenderComponent rc;
    rc.shape = sf::CircleShape(cfg.L);
    rc.shape.setOrigin(rc.shape.getRadius(), rc.shape.getRadius());
    components.render_comps[player_id] = rc;
    PhysicsComponent pc;
    pc.pos = sf::Vector2f(0.f, 0.f);
    pc.vel = sf::Vector2f(0.f, 0.f);
    components.physics_comps[player_id] = pc;

    // Make beacon particle for player
    EntityId beacon_id = createEntity();
    MetadataComponent mc_beacon;
    mc_beacon.id = beacon_id;
    mc_beacon.name = "Beacon";
    components.metadata_comps[beacon_id] = mc_beacon;
    RenderComponent rc_beacon;
    rc_beacon.shape = sf::CircleShape(cfg.L / 2.f);
    rc_beacon.shape.setOrigin(rc_beacon.shape.getRadius(), rc_beacon.shape.getRadius());
    rc_beacon.shape.setFillColor(sf::Color::Black);
    rc_beacon.shape.setOutlineColor(sf::Color::White);
    rc_beacon.shape.setOutlineThickness(cfg.L / 10.f);
    components.render_comps[beacon_id] = rc_beacon;
    PhysicsComponent pc_beacon;
    pc_beacon.pos = sf::Vector2f(0.f, 0.f);
    pc_beacon.vel = sf::Vector2f(0.f, 0.f);
    components.physics_comps[beacon_id] = pc_beacon;
    // EntityId rel_beacon_id = createEntityRelationship();
    // PairwiseForceComponent pfc_beacon;
    // pfc_beacon.target_entity = player_id;
    // pfc_beacon.source_entity = beacon_id;
    // pfc_beacon.params.magnitude *= -0.1f;
    // components.pairforce_comps[rel_beacon_id] = pfc_beacon;

    // Make enemies
    std::random_device rd;
    std::mt19937 gen(rd());  // Standard random number generator
    std::uniform_real_distribution<float> dist(-10.f * cfg.L, 10.f * cfg.L);
    int n_enemies = 10;
    for (int i = 0; i < n_enemies; ++i) {
        // Entity properties
        EntityId id = createEntity();
        MetadataComponent mc;
        mc.id = id;
        mc.name = "Enemy" + std::to_string(i);
        components.metadata_comps[id] = mc;
        // Randomly distributed in a square
        PhysicsComponent pc;
        pc.pos = sf::Vector2f(dist(gen), dist(gen) - cfg.window_size.y / 2.f);
        pc.vel = sf::Vector2f(0.f, 0.f);
        components.physics_comps[id] = pc;
        // Colored circles
        RenderComponent rc;
        rc.shape = sf::CircleShape(cfg.L);
        rc.shape.setFillColor(sf::Color::Red);
        rc.shape.setOrigin(rc.shape.getRadius(), rc.shape.getRadius());
        rc.shape.setPosition(pc.pos);
        components.render_comps[id] = rc;

        // Relationship with other entities
        // Pulled towards the player
        EntityId rel_id = createEntityRelationship();
        PairwiseForceComponent pfc;
        pfc.target_entity = id;
        pfc.source_entity = player_id;
        // Because of the r^-2 force drops off quickly if we don't scale it strongly
        // pfc.params.magnitude *= -100.f * cfg.A;
        // pfc.params.power = -2.f;
        // pfc.params.softening = cfg.L;
        // Alternate setup: springs. TODO: Add friction or this goes crazy.
        pfc.params.magnitude *= -0.1f;
        components.pairforce_comps[rel_id] = pfc;
        // Collides with the player
        CollisionComponent cc;
        cc.id1 = id;
        cc.id2 = player_id;
        components.collision_comps[rel_id] = cc;
    }
    // Enemies collide with each other
    for (auto& [id1, pc1] : components.physics_comps) {
        if ((id1 == player_id) || (id1 == beacon_id)) {
            continue;
        }
        for (auto& [id2, pc2] : components.physics_comps) {
            if ((id1 == id2) || (id2 == player_id) || (id2 == beacon_id)) {
                continue;
            }
            EntityId rel_id = createEntityRelationship();
            CollisionComponent cc;
            cc.id1 = id1;
            cc.id2 = id2;
            components.collision_comps[rel_id] = cc;
        }
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
        rc.shape.setPosition(0.f, 0.f);
        rc.zorder = -i;
        components.render_comps[id] = rc;
        // PhysicsComponent pc;
        // pc.pos = sf::Vector2f(0.f, 0.f);
        // pc.vel = sf::Vector2f(0.f, 0.f);
        // components.physics_comps[id] = pc;
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

    // Resolve collisions
    physics_system.resolveCollisions(components);
}

void Game::render() {
    // Pin the view to the player
    view.setCenter(components.physics_comps.at(player_id).pos);
    window.setView(view);

    // Render
    render_system.render(window, components);
}