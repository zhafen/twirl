#include "config.h"
#include "game.h"

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <filesystem>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "shape.h"
#include "system.h"

namespace twirl {

Game::Game()
    : window(sf::VideoMode(cfg.window_size_x, cfg.window_size_y), "twirl"),
      view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size_x, cfg.window_size_y)),
      ui_view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size_x, cfg.window_size_y)),
      registry(),
      render_system(view, ui_view),
      entity_system(),
      physics_system() {
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
    // Delete all entities
    registry.clear();

    // Reinitialize the game state
    initializeState();
}

void Game::initializeState() {
    // Make player
    player = registry.create();
    registry.emplace<PhysicsComp>(player);
    registry.emplace<DragForceComp>(player);
    auto& dc = registry.emplace<DurabilityComp>(player);
    dc.delete_at_zero = false;
    auto& player_rc = registry.emplace<RenderComp>(player, TwirlCircleShape(cfg.L));
    player_rc.shape.setFillColor(sf::Color::White);
    // All spawn comps must be accompanied by a StopWatchComp that regulates
    // the spawn rate
    registry.emplace<SpawnComp>(player);
    registry.emplace<StopWatchComp>(player);

    // Make beacon particle for player
    const auto beacon = registry.create();
    registry.emplace<PhysicsComp>(beacon);
    registry.emplace<MouseButtonReleasedComp>(beacon);
    auto beacon_shape = TwirlCircleShape(cfg.L / 2.f);
    beacon_shape.setFillColor(sf::Color::Black);
    beacon_shape.setOutlineColor(sf::Color::White);
    beacon_shape.setOutlineThickness(cfg.L / 10.f);
    registry.emplace<RenderComp>(beacon, beacon_shape, 1);  // zorder = 1

    // Make an entity for the relationship between the player and the beacon
    const auto rel_beacon = registry.create();
    // This component tracks the relationship itself
    registry.emplace<PairComp>(rel_beacon, player, beacon);
    auto& pfc = registry.emplace<PairwiseForceComp>(rel_beacon);
    pfc.magnitude = -0.1f;

    // Make enemies
    std::random_device rd;
    std::mt19937 gen(rd());  // Standard random number generator
    std::uniform_real_distribution<float> dist(-10.f * cfg.L, 10.f * cfg.L);
    std::size_t n_enemies = 10;
    std::vector<entt::entity> enemy_ids;
    for (int i = 0; i < n_enemies; ++i) {
        // Entity properties
        auto enemy = registry.create();
        // Flag as enemies
        registry.emplace<EnemyComp>(enemy);
        // They have durability
        registry.emplace<DurabilityComp>(enemy);
        // Randomly distributed in a square
        auto& pc = registry.emplace<PhysicsComp>(enemy);
        pc.pos = sf::Vector2f(dist(gen), dist(gen) - cfg.window_size_y / 2.f);
        pc.vel = sf::Vector2f(0.f, 0.f);
        // Affected by drag;
        registry.emplace<DragForceComp>(enemy);
        // Colored circles
        auto& rc = registry.emplace<RenderComp>(enemy);
        rc.shape = TwirlCircleShape(cfg.L);
        rc.shape.setFillColor(sf::Color::Red);
        rc.shape.setPosition(pc.pos);

        // Relationship with other entities
        // Pulled towards the player
        auto relation = registry.create();
        // First force: gravity
        // Because of the r^-2 force drops off quickly if we don't scale it strongly
        auto& prc = registry.emplace<PairComp>(relation, enemy, player);
        auto& pfc = registry.emplace<PairwiseForceComp>(relation);
        pfc.magnitude = -1.0f;
        pfc.power = -2.0f;
        pfc.softening = 1.0f;
        pfc.distance_scaling = cfg.window_size_x / 2.0f / cfg.L;
        // Second force: springs
        auto relation2 = registry.create();
        auto& prc2 = registry.emplace<PairComp>(relation2, enemy, player);
        auto& pfc2 = registry.emplace<PairwiseForceComp>(relation2);
        pfc2.magnitude = -0.1f;
        // Collides with the player
        auto col_id = registry.create();
        registry.emplace<CollisionComp>(col_id, enemy, player);

        // Store the enemy id
        enemy_ids.push_back(enemy);
    }
    // Enemies collide with each other
    for (auto& entity1 : enemy_ids) {
        for (auto& entity2 : enemy_ids) {
            if (entity1 == entity2 || entity1 > entity2) {
            continue;
            }
            auto relation = registry.create();
            registry.emplace<PairComp>(relation, entity1, entity2);
            registry.emplace<CollisionComp>(relation);
        }
    }

    // Make background
    int n_bkgrd = 100;
    for (int i = 0; i < n_bkgrd; ++i) {
        auto bkgrd = registry.create();
        registry.emplace<PhysicsComp>(bkgrd);
        TwirlCircleShape shape(10.f * cfg.L * i);
        shape.setFillColor(sf::Color(127, 127, 127));
        shape.setOutlineThickness(cfg.L / 5.f);
        shape.setOutlineColor(sf::Color(63, 63, 63));
        shape.setPosition(0.f, 0.f);
        registry.emplace<RenderComp>(bkgrd, shape, -i);
    }

    // Add a durability bar
    auto bar = registry.create();
    // Have to pass in a shape to the component or it crashes, unlike with RenderComp
    // Not sure why
    auto& uic_bar = registry.emplace<UIComp>(bar, sf::RectangleShape());
    uic_bar.shape.setFillColor(sf::Color::White);
    uic_bar.shape.setOutlineThickness(cfg.L / 10.f);
    uic_bar.shape.setOutlineColor(sf::Color::Black);
    // This is the key part where the tracked value is set
    uic_bar.tracked_value = &registry.get<DurabilityComp>(player).durability;
    // Have to convert the shape to a rectangle to set the size
    uic_bar.size = sf::Vector2f(cfg.window_size_x / 2, cfg.L);
    uic_bar.pos =
        sf::Vector2f(-uic_bar.size.x / 2.f, -float(cfg.window_size_y) / 2.f + cfg.L);
}

void Game::handleEvents() {
    for (auto event = sf::Event(); window.pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::MouseButtonReleased) {
            auto rview = registry.view<PhysicsComp, MouseButtonReleasedComp>();
            for (auto& entity : rview) {
                // get the current mouse position in the window
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

                // convert it to world coordinates and store
                auto& pc = registry.get<PhysicsComp>(entity);
                pc.pos = window.mapPixelToCoords(pixelPos);
            }
        } else if (event.type == sf::Event::KeyReleased &&
                   event.key.code == sf::Keyboard::R) {
            resetGameState();
        }
    }
}

    void Game::update() {
        // Spawn and despawn entities
        entity_system.deleteEntities(registry);
        entity_system.spawnEntities(registry);
        entity_system.orderEntities(registry);

        // Calculate forces
        physics_system.calculateForces(registry);
        physics_system.calculatePairwiseForces(registry);

        // Update state
        physics_system.update(registry);
        physics_system.updateStopWatches(registry);

        // Resolve collisions
        physics_system.resolveCollisions(registry);
        physics_system.updateDurability(registry);
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

}  // namespace twirl