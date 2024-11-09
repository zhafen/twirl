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
      entity_system(cfg),
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
    player = registry.create();
    registry.emplace<PhysicsComp>(player);
    registry.emplace<ForceComp>(player);
    registry.emplace<DurabilityComp>(player);
    auto &player_rc = registry.emplace<RenderComp>(player, CCCircleShape(cfg.L));
    player_rc.shape.setFillColor(sf::Color::White);

    // Make beacon particle for player
    const auto beacon = registry.create();
    registry.emplace<PhysicsComp>(beacon);
    registry.emplace<MouseButtonReleasedComp>(beacon);
    auto beacon_shape = CCCircleShape(cfg.L / 2.f);
    beacon_shape.setFillColor(sf::Color::Black);
    beacon_shape.setOutlineColor(sf::Color::White);
    beacon_shape.setOutlineThickness(cfg.L / 10.f);
    registry.emplace<RenderComp>(beacon, beacon_shape, 1); // zorder = 1

    // Make an entity for the relationship between the player and the beacon
    const auto rel_beacon = registry.create();
    // This component tracks the relationship itself
    registry.emplace<PairComp>(rel_beacon, player, beacon);
    auto& pfc = registry.emplace<PairwiseForceComp>(rel_beacon);
    pfc.magnitude = -0.1f;

    // // Make enemies
    // std::random_device rd;
    // std::mt19937 gen(rd());  // Standard random number generator
    // std::uniform_real_distribution<float> dist(-10.f * cfg.L, 10.f * cfg.L);
    // int n_enemies = 10;
    // std::vector<EntityId> enemy_ids;
    // for (int i = 0; i < n_enemies; ++i) {
    //     // Entity properties
    //     EntityId id = createEntity();
    //     mc.id = id;
    //     mc.name = "Enemy" + std::to_string(i);
    //     components.metadata_comps[id] = mc;
    //     // Randomly distributed in a square
    //     PhysicsComp pc;
    //     pc.pos = sf::Vector2f(dist(gen), dist(gen) - cfg.window_size_y / 2.f);
    //     pc.vel = sf::Vector2f(0.f, 0.f);
    //     components.physics_comps[id] = pc;
    //     // Affected by drag;
    //     components.force_comps[id] = ForceComp();
    //     // Colored circles
    //     RenderComp rc;
    //     rc.shape = std::make_shared<CCCircleShape>(cfg.L);
    //     rc.shape->setFillColor(sf::Color::Red);
    //     rc.shape->setPosition(pc.pos);
    //     components.render_comps[id] = std::move(rc);

    //     // Relationship with other entities
    //     // Pulled towards the player
    //     EntityRelationId rel_id = createEntityRelationship();
    //     // First force: gravity
    //     // Because of the r^-2 force drops off quickly if we don't scale it strongly
    //     PairwiseForceComp pfc;
    //     pfc.target_entity = id;
    //     pfc.source_entity = player_id;
    //     pfc.params.magnitude = -1.0f;
    //     pfc.params.power = -2.0f;
    //     pfc.params.softening = 1.0f;
    //     pfc.params.distance_scaling = cfg.window_size_x / 2.0f / cfg.L;
    //     components.pairforce_comps[rel_id] = pfc;
    //     // Second force: springs
    //     EntityRelationId rel_id2 = createEntityRelationship();
    //     PairwiseForceComp pfc2;
    //     pfc2.target_entity = id;
    //     pfc2.source_entity = player_id;
    //     pfc2.params.magnitude = -0.1f;
    //     components.pairforce_comps[rel_id2] = pfc2;
    //     // Collides with the player
    //     CollisionComp cc;
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
    //         CollisionComp cc;
    //         cc.id1 = id1;
    //         cc.id2 = id2;
    //         components.collision_comps[rel_id] = cc;
    //     }
    // }

    // Make background
    int n_bkgrd = 100;
    for (int i = 0; i < n_bkgrd; ++i) {
        auto bkgrd = registry.create();
        registry.emplace<PhysicsComp>(bkgrd);
        CCCircleShape shape(10.f * cfg.L * i);
        shape.setFillColor(sf::Color(127, 127, 127));
        shape.setOutlineThickness(cfg.L / 5.f);
        shape.setOutlineColor(sf::Color(63, 63, 63));
        shape.setPosition(0.f, 0.f);
        auto& rc = registry.emplace<RenderComp>(bkgrd, shape, -i);
    }

    // // Add a durability bar
    // EntityId bar_id = createEntity();
    // UIComp uic_bar(registry.dura_comps.at(player_id).durability);
    // uic_bar.shape = std::make_shared<sf::RectangleShape>(sf::Vector2f(100.f, 10.f));
    // uic_bar.shape->setFillColor(sf::Color::White);
    // uic_bar.shape->setOutlineThickness(cfg.L / 10.f);
    // uic_bar.shape->setOutlineColor(sf::Color::Black);
    // uic_bar.size = sf::Vector2f(cfg.window_size_x / 2, cfg.L);
    // // Have to convert the shape to a rectangle to set the size
    // uic_bar.pos =
    //     sf::Vector2f(-uic_bar.size.x / 2.f, -float(cfg.window_size_y) / 2.f + cfg.L);
    // components.ui_comps.emplace(bar_id, std::move(uic_bar));

    // Create a vector of (zorder, id) pairs
    // for (const auto& [id, rc] : components.render_comps) {
    //     components.rc_zorders.emplace_back(rc.zorder, id);
    // }
    // // Sort the vector according to zorder
    // std::sort(registry.rc_zorders.begin(), components.rc_zorders.end());

    // // Do the same for the UI
    // for (const auto& [id, uic] : components.ui_comps) {
    //     components.uic_zorders.emplace_back(uic.zorder, id);
    // }
    // // Sort the vector according to zorder
    // std::sort(registry.uic_zorders.begin(), components.uic_zorders.end());
}

void Game::handleEvents() {
    for (auto event = sf::Event(); window.pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::MouseButtonReleased) {
            auto rview = registry.view<PhysicsComp, MouseButtonReleasedComp>();
            for (auto& entity: rview) {
                // get the current mouse position in the window
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

                // convert it to world coordinates and store
                auto& pc = registry.get<PhysicsComp>(entity);
                pc.pos = window.mapPixelToCoords(pixelPos);
            }
        }
    }
}

void Game::update() {
    // Spawn objects
    entity_system.orderEntities(registry);

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
    render_system.render(registry, window);
    render_system.renderUI(window, registry);

    // Pin the view to the player
    view.setCenter(registry.get<PhysicsComp>(player).pos);
    window.setView(view);

    window.display();
}

}  // namespace cc