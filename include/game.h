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
    Game();
    void run();
    void resetGameState();

   private:
    // Window and view
    sf::RenderWindow window;
    sf::View view;
    sf::View ui_view;

    // ECS
    entt::registry registry;
    entt::entity player;

    // Systems
    SceneSystem scene_system;
    EntitySystem entity_system;
    RenderSystem render_system;
    PhysicsSystem physics_system;

    // Methods
    void initializeState();
    void handleEvents();
    void update();
    void render();
};

}  // namespace twirl

#endif  // GAME_H