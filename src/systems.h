#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SFML/Graphics.hpp>

#include "game_objects.h"

using EntityId = int;

struct RenderComponent {
    sf::CircleShape shape;
    int zorder = 0;
};

struct PhysicsComponent {
    sf::Vector2f pos;
    sf::Vector2f vel;
};

struct Components {
    std::unordered_map<EntityId, RenderComponent> render_comps;
    std::unordered_map<EntityId, PhysicsComponent> physics_comps;
    std::vector<std::pair<int, EntityId>> entity_zorders;
};

class RenderSystem {
   public:
    RenderSystem(const Config& cfg, sf::View& view);
    void render(sf::RenderWindow& window, Components& components);

   private:
    sf::View view;
    Config cfg;
};

#endif  // SYSTEMS_H