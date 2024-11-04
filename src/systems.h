#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SFML/Graphics.hpp>

#include "game_objects.h"

using EntityId = int;

struct RenderComponent {
    sf::CircleShape shape;
};

struct PhysicsComponent {
    sf::Vector2f pos;
    sf::Vector2f vel;
};

class RenderSystem {
   public:
    RenderSystem(const Config& cfg, sf::View& view);
    void render(
        sf::RenderWindow& window,
        std::unordered_map<EntityId, RenderComponent>& render_components,
        std::unordered_map<EntityId, PhysicsComponent>& physics_components);

   private:
    sf::View view;
    Config cfg;
};

#endif  // SYSTEMS_H