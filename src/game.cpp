#include "game.h"

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "shape.h"
#include "system.h"

namespace cc {

Game::Game()
    : cfg(),
      window(sf::VideoMode(cfg.window_size_x, cfg.window_size_y), "twirl"),
      view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size_x, cfg.window_size_y)),
      ui_view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size_x, cfg.window_size_y)),
      registry(),
      render_system(cfg, view, ui_view),
      physics_system(cfg),
      general_system(cfg) {
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

// int Game::createEntity() { return entityCounter++; }
// Same as createEntity, but for relationships between entities
// int Game::createEntityRelationship() { return entityRelationshipCounter++; }

void Game::initializeState() {
    // Make player
    const auto player = registry.create();
    registry.emplace<MetadataComponent>(player, "Player");
    auto &player_pc = registry.emplace<PhysicsComponent>(player);
    player_pc.pos = sf::Vector2f(0.f, 0.f);
    player_pc.vel = sf::Vector2f(0.f, 0.f);
    registry.emplace<ForceComponent>(player);
    registry.emplace<DurabilityComponent>(player);
    auto &player_rc = registry.emplace<RenderComponent>(player, CCCircleShape(cfg.L));
    player_rc.shape.setFillColor(sf::Color::White);

    // // Make beacon particle for player
    // EntityId beacon_id = createEntity();
    // MetadataComponent mc_beacon;
    // mc_beacon.id = beacon_id;
    // mc_beacon.name = "Beacon";
    // components.metadata_comps[beacon_id] = mc_beacon;
    // RenderComponent rc_beacon;
    // rc_beacon.shape = std::make_shared<CCCircleShape>(cfg.L / 2.f);
    // rc_beacon.shape->setFillColor(sf::Color::Black);
    // rc_beacon.shape->setOutlineColor(sf::Color::White);
    // rc_beacon.shape->setOutlineThickness(cfg.L / 10.f);
    // components.render_comps[beacon_id] = std::move(rc_beacon);
    // PhysicsComponent pc_beacon;
    // pc_beacon.pos = sf::Vector2f(0.f, 0.f);
    // pc_beacon.vel = sf::Vector2f(0.f, 0.f);
    // components.physics_comps[beacon_id] = pc_beacon;
    // MouseButtonReleasedComponent mbrc;
    // components.mousebuttonreleased_comps[beacon_id] = mbrc;
    // // Pull the player towards the beacon
    // EntityRelationId rel_beacon_id = createEntityRelationship();
    // PairwiseForceComponent pfc_beacon;
    // pfc_beacon.target_entity = player_id;
    // pfc_beacon.source_entity = beacon_id;
    // pfc_beacon.params.magnitude *= -0.1f;
    // components.pairforce_comps[rel_beacon_id] = pfc_beacon;

    // // Make enemies
    // std::random_device rd;
    // std::mt19937 gen(rd());  // Standard random number generator
    // std::uniform_real_distribution<float> dist(-10.f * cfg.L, 10.f * cfg.L);
    // int n_enemies = 10;
    // std::vector<EntityId> enemy_ids;
    // for (int i = 0; i < n_enemies; ++i) {
    //     // Entity properties
    //     EntityId id = createEntity();
    //     MetadataComponent mc;
    //     mc.id = id;
    //     mc.name = "Enemy" + std::to_string(i);
    //     components.metadata_comps[id] = mc;
    //     // Randomly distributed in a square
    //     PhysicsComponent pc;
    //     pc.pos = sf::Vector2f(dist(gen), dist(gen) - cfg.window_size_y / 2.f);
    //     pc.vel = sf::Vector2f(0.f, 0.f);
    //     components.physics_comps[id] = pc;
    //     // Affected by drag;
    //     components.force_comps[id] = ForceComponent();
    //     // Colored circles
    //     RenderComponent rc;
    //     rc.shape = std::make_shared<CCCircleShape>(cfg.L);
    //     rc.shape->setFillColor(sf::Color::Red);
    //     rc.shape->setPosition(pc.pos);
    //     components.render_comps[id] = std::move(rc);

    //     // Relationship with other entities
    //     // Pulled towards the player
    //     EntityRelationId rel_id = createEntityRelationship();
    //     // First force: gravity
    //     // Because of the r^-2 force drops off quickly if we don't scale it strongly
    //     PairwiseForceComponent pfc;
    //     pfc.target_entity = id;
    //     pfc.source_entity = player_id;
    //     pfc.params.magnitude = -1.0f;
    //     pfc.params.power = -2.0f;
    //     pfc.params.softening = 1.0f;
    //     pfc.params.distance_scaling = cfg.window_size_x / 2.0f / cfg.L;
    //     components.pairforce_comps[rel_id] = pfc;
    //     // Second force: springs
    //     EntityRelationId rel_id2 = createEntityRelationship();
    //     PairwiseForceComponent pfc2;
    //     pfc2.target_entity = id;
    //     pfc2.source_entity = player_id;
    //     pfc2.params.magnitude = -0.1f;
    //     components.pairforce_comps[rel_id2] = pfc2;
    //     // Collides with the player
    //     CollisionComponent cc;
    //     cc.id1 = id;
    //     cc.id2 = player_id;
    //     components.collision_comps[rel_id] = cc;

    //     // Store the enemy id
    //     enemy_ids.push_back(id);
    // }
    // // Enemies collide with each other
    // for (auto& id1 : enemy_ids) {
    //     for (auto& id2 : enemy_ids) {
    //         if (id1 == id2) {
    //             continue;
    //         }
    //         EntityId rel_id = createEntityRelationship();
    //         CollisionComponent cc;
    //         cc.id1 = id1;
    //         cc.id2 = id2;
    //         components.collision_comps[rel_id] = cc;
    //     }
    // }

    // // Make background
    // int n_bkgrd = 100;
    // for (int i = 0; i < n_bkgrd; ++i) {
    //     EntityId id = createEntity();
    //     RenderComponent rc;
    //     rc.shape = std::make_shared<CCCircleShape>(10.f * cfg.L * i);
    //     rc.shape->setFillColor(sf::Color(127, 127, 127));
    //     rc.shape->setOutlineThickness(cfg.L / 5.f);
    //     rc.shape->setOutlineColor(sf::Color(63, 63, 63));
    //     rc.shape->setPosition(0.f, 0.f);
    //     rc.zorder = -i;
    //     components.render_comps[id] = std::move(rc);
    // }

    // // Add a durability bar
    // EntityId bar_id = createEntity();
    // UIComponent uic_bar(registry.dura_comps.at(player_id).durability);
    // uic_bar.shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(100.f, 10.f));
    // uic_bar.shape->setFillColor(sf::Color::White);
    // uic_bar.shape->setOutlineThickness(cfg.L / 10.f);
    // uic_bar.shape->setOutlineColor(sf::Color::Black);
    // uic_bar.size = sf::Vector2f(cfg.window_size_x / 2, cfg.L);
    // // Have to convert the shape to a rectangle to set the size
    // uic_bar.pos =
    //     sf::Vector2f(-uic_bar.size.x / 2.f, -float(cfg.window_size_y) / 2.f + cfg.L);
    // components.ui_comps.emplace(bar_id, std::move(uic_bar));

    // // Create a vector of (zorder, id) pairs
    // for (const auto& [id, rc] : components.render_comps) {
    //     components.entity_zorders.emplace_back(rc.zorder, id);
    // }
    // // Sort the vector according to zorder
    // std::sort(registry.entity_zorders.begin(), components.entity_zorders.end());

    // // Do the same for the UI
    // for (const auto& [id, uic] : components.ui_comps) {
    //     components.ui_entity_zorders.emplace_back(uic.zorder, id);
    // }
    // // Sort the vector according to zorder
    // std::sort(registry.ui_entity_zorders.begin(), components.ui_entity_zorders.end());
}

void Game::handleEvents() {
    for (auto event = sf::Event(); window.pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::MouseButtonReleased) {
            // for (auto& [id, mbrc] : components.mousebuttonreleased_comps) {
            //     // get the current mouse position in the window
            //     sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

            //     // convert it to world coordinates and store
            //     auto& pc = components.physics_comps.at(id);
            //     pc.pos = window.mapPixelToCoords(pixelPos);
            // }
        }
    }
}

void Game::update() {
    // Spawn objects

    // Calculate forces
    physics_system.calculateForces(registry);
    physics_system.calculatePairwiseForces(registry);

    // Update state
    physics_system.update(registry);

    // Resolve collisions
    physics_system.resolveCollisions(registry);
    physics_system.updateDurability(registry);

    // Call pairwise functions
    general_system.callPairwiseFunctions(registry);
}

void Game::render() {
    // Render
    render_system.render(player, window, registry);
    // render_system.renderUI(window, registry);

    // Pin the view to the player
    view.setCenter(registry.get<PhysicsComponent>(player).pos);
    window.setView(view);

    window.display();
}

}  // namespace cc