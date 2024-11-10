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
    registry.emplace<DragForceComp>(player);
    registry.emplace<DurabilityComp>(player);
    auto& player_rc = registry.emplace<RenderComp>(player, CCCircleShape(cfg.L));
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
    auto& pfc = registry.emplace<PairwiseForceComp>(rel_beacon, player, beacon);
    pfc.params.magnitude = -0.1f;

    // Make enemies
    std::random_device rd;
    std::mt19937 gen(rd());  // Standard random number generator
    std::uniform_real_distribution<float> dist(-10.f * cfg.L, 10.f * cfg.L);
    int n_enemies = 10;
    std::vector<entt::entity> enemy_ids;
    for (int i = 0; i < n_enemies; ++i) {
        // Entity properties
        auto enemy = registry.create();
        // Randomly distributed in a square
        auto& pc = registry.emplace<PhysicsComp>(enemy);
        pc.pos = sf::Vector2f(dist(gen), dist(gen) - cfg.window_size_y / 2.f);
        pc.vel = sf::Vector2f(0.f, 0.f);
        // Affected by drag;
        registry.emplace<DragForceComp>(enemy);
        // Colored circles
        auto& rc = registry.emplace<RenderComp>(enemy);
        rc.shape = CCCircleShape(cfg.L);
        rc.shape.setFillColor(sf::Color::Red);
        rc.shape.setPosition(pc.pos);

        // Relationship with other entities
        // Pulled towards the player
        auto relation = registry.create();
        // First force: gravity
        // Because of the r^-2 force drops off quickly if we don't scale it strongly
        auto& pfc = registry.emplace<PairwiseForceComp>(relation, enemy, player);
        pfc.params.magnitude = -1.0f;
        pfc.params.power = -2.0f;
        pfc.params.softening = 1.0f;
        pfc.params.distance_scaling = cfg.window_size_x / 2.0f / cfg.L;
        // Second force: springs
        auto relation2 = registry.create();
        auto& pfc2 = registry.emplace<PairwiseForceComp>(relation2);
        pfc2.target_entity = enemy;
        pfc2.source_entity = player;
        pfc2.params.magnitude = -0.1f;
        // Collides with the player
        auto col_id = registry.create();
        registry.emplace<CollisionComp>(col_id, enemy, player);

        // Store the enemy id
        enemy_ids.push_back(enemy);
    }
    // Enemies collide with each other
    for (auto& entity1 : enemy_ids) {
        for (auto& entity2 : enemy_ids) {
            if (entity1 == entity2) {
                continue;
            }
            auto relation = registry.create();
            registry.emplace<CollisionComp>(relation, entity1, entity2);
        }
    }

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