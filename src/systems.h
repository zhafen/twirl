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
    float mass = 1.0f;
    sf::Vector2f pos = {0.0f, 0.0f};
    sf::Vector2f vel = {0.0f, 0.0f};
    sf::Vector2f force = {0.0f, 0.0f};
};

struct PairwiseForceComponent {
    EntityId target_entity;
    EntityId source_entity;
    struct Parameters {
        float magnitude = 1.0f;
        float softening = 0.0f;
        float power = 2.0f;
    } params;
};

struct Components {
    std::unordered_map<EntityId, RenderComponent> render_comps;
    std::unordered_map<EntityId, PhysicsComponent> physics_comps;
    std::unordered_map<EntityId, PairwiseForceComponent> pairforce_comps;
    std::vector<std::pair<int, EntityId>> entity_zorders;
};

class PhysicsSystem {
   public:
    PhysicsSystem(const Config& cfg);
    void calculateForces(Components& components);
    void update(Components& components);
   private:
    Config cfg;
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