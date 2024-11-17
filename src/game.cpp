
#include "game.h"

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <filesystem>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "config.h"
#include "scene.h"
#include "shape.h"
#include "system.h"

namespace twirl {

Game::Game(std::string main_scene_fp)
    : main_scene_fp(main_scene_fp),
      window(sf::VideoMode(cfg.window_size_x, cfg.window_size_y), "twirl"),
      view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size_x, cfg.window_size_y)),
      ui_view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size_x, cfg.window_size_y)),
      registry(),
      render_system(view, ui_view),
      entity_system(),
      physics_system() {
    window.setFramerateLimit(cfg.fps);
    initialize();
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::initialize() {
    // Emplace the main scene
    entt::entity scene_entity = registry.create();
    registry.emplace<SceneComp>(scene_entity, main_scene_fp);
    registry.emplace<EntityName>(scene_entity, "main_scene");
    scene_system.loadJsonData(registry);
    scene_system.emplaceScene(registry, scene_entity);

    // Get the entity map at the start
    // The entity map is a mapping of entity names to entity IDs
    // It's used when loading scenes from json
    entity_map = entity_system.getEntityMap(registry);

    // Load other scenes
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
    // Try spawning, deleting, and ordering entities
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
    render_system.renderUI(registry, window);
    render_system.setView(registry, window, view);

    window.display();
}

}  // namespace twirl