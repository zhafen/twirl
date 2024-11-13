#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>

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

    // Entities
    // EntityId entityCounter = 0;
    // EntityId entityRelationshipCounter = 0;
    entt::entity player;
    // std::vector<sf::CircleShape> bkgrd_circles;

    // UI Elements
    sf::Text announcement;

    // Systems
    EntitySystem entity_system;
    RenderSystem render_system;
    PhysicsSystem physics_system;

    // Methods
    int createEntity();
    int createEntityRelationship();
    void initializeState();
    void initializeStateOld();
    void handleEvents();
    void update();
    void render();
};

}  // namespace twirl

#endif  // GAME_H