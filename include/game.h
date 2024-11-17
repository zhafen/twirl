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

    // Most of our methods are public
    void run();
    void resetGameState();
    void handleEvents();
    void update();
    void render();

    // Similarly, our registry is public
    entt::registry registry;

   private:
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