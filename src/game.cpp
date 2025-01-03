
#include "game.h"

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <filesystem>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "config.h"
#include "shape.h"
#include "systems/system.h"

namespace twirl {

Game::Game(std::string main_scene_fp)
    : main_scene_fp(main_scene_fp),
      window(sf::VideoMode(cfg.window_size_x, cfg.window_size_y), "twirl"),
      view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size_x, cfg.window_size_y)),
      ui_view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size_x, cfg.window_size_y)),
      registry(),
      render_system(view, ui_view),
      entity_system(),
      interface_system(),
      physics_system() {
    window.setFramerateLimit(cfg.fps);
    initialize();
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        if (!interface_system.isGameSuspended(registry)) {
            update();
        }
        render();
        time_since_start += cfg.dt;
        if (time_since_start > max_time) {
            break;
        }
    }
}

void Game::initialize() {
    // Hook up the scene trigger listener
    registry.on_update<TriggerComp>().connect<&SceneSystem::onSceneTrigger>(
        scene_system);

    // Emplace the main scene
    entt::entity scene_entity = registry.create();
    auto& scene_c = registry.emplace<SceneComp>(scene_entity, main_scene_fp);
    registry.emplace<EntityName>(scene_entity, "main_scene");
    scene_system.loadJsonData(registry);
    scene_system.emplaceScene(registry, scene_entity);

    // Load other scenes.
    // We sort by emplace order first.
    registry.sort<SceneComp>([](const auto lhs, const auto rhs) {
        return lhs.emplace_order < rhs.emplace_order;
    });
    scene_system.loadJsonData(registry);
}

void Game::resetGameState() {
    // Delete all entities
    registry.clear();

    // Reinitialize the game state
    initialize();
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
                auto& phys_c = registry.get<PhysicsComp>(entity);
                phys_c.pos = window.mapPixelToCoords(pixelPos);
            }
        } else if (event.type == sf::Event::KeyReleased &&
                   event.key.code == sf::Keyboard::R) {
            resetGameState();
        }
    }
}

void Game::update() {
    // Delete and emplace entities
    entity_system.deleteEntities(registry);
    bool scene_emplaced = scene_system.checkTriggers(registry);
    if (scene_emplaced) {
        entity_system.needs_ordering = true;
    }

    // Order and sync entities
    entity_system.orderEntities(registry);
    entity_system.syncEntities(registry);

    // Calculate forces
    physics_system.calculateForces(registry);
    physics_system.calculatePairwiseForces(registry);

    // Update state
    physics_system.update(registry);
    physics_system.updateStopWatches(registry);

    // Resolve collisions
    physics_system.resolveCollisions(registry);
    physics_system.updateDurability(registry);

    // Debug when such components exist
    comp::debugEntities(registry, "\nDebug: End of update");
}

void Game::render() {
    // Render
    render_system.prepareRender(registry, window);
    render_system.render(registry, window);
    render_system.renderUI(registry, window);
    render_system.setView(registry, window, view);

    // Debug when such components exist
    comp::debugEntities(registry, "\nDebug: End of render");

    window.display();
}

}  // namespace twirl