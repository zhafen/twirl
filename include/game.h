#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "scene.h"
#include "system.h"


namespace twirl {

class Game {
   public:
    Game(std::string main_scene_fp);

    // Most of our methods are public
    void run();
    void resetGameState();
    void handleEvents();
    void update();
    void render();

    // Getter functions for private variables
    entt::registry& getRegistry() { return registry; }
    std::unordered_map<std::string, entt::entity>& getEntityMap() { return entity_map; }
    sf::RenderWindow& getWindow() { return window; }

   private:
    std::string main_scene_fp;

    // ECS
    entt::registry registry;
    std::unordered_map<std::string, entt::entity> entity_map;

    // Window and view
    sf::RenderWindow window;
    sf::View view;
    sf::View ui_view;

    // Systems
    SceneSystem scene_system;
    EntitySystem entity_system;
    RenderSystem render_system;
    PhysicsSystem physics_system;

    // Methods
    void initialize();
};

}  // namespace twirl

#endif  // GAME_H