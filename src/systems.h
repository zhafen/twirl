#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <memory>
#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "game_objects.h"

using EntityId = int;
using EntityRelationId = int;

struct MetadataComponent {
    EntityId id;
    std::string name;
};

struct RenderComponent {
    std::unique_ptr<sf::Shape> shape;
    int zorder = 0;
};

struct PhysicsComponent {
    float mass = 1.0f;
    float durability = 1.0f;
    sf::Vector2f pos = {0.0f, 0.0f};
    sf::Vector2f vel = {0.0f, 0.0f};
    sf::Vector2f force = {0.0f, 0.0f};
};

struct MouseButtonReleasedComponent {};

struct PairwiseForceComponent {
    EntityId target_entity;
    EntityId source_entity;
    struct Parameters {
        float magnitude = 1.0f;
        float softening = 0.0f;
        float power = 2.0f;
        float scaled_min_distance = 0.1f;
    } params;
};

struct CollisionComponent {
    EntityId id1;
    EntityId id2;
};

struct Components {
    // Single-entity components
    std::unordered_map<EntityId, MetadataComponent> metadata_comps;
    std::unordered_map<EntityId, RenderComponent> render_comps;
    std::unordered_map<EntityId, PhysicsComponent> physics_comps;
    std::unordered_map<EntityId, MouseButtonReleasedComponent>
        mousebuttonreleased_comps;
    std::vector<std::pair<int, EntityId>> entity_zorders;

    // Multi-entity components
    std::unordered_map<EntityRelationId, PairwiseForceComponent> pairforce_comps;
    std::unordered_map<EntityRelationId, CollisionComponent> collision_comps;
};

class PhysicsSystem {
   public:
    PhysicsSystem(const Config& cfg);
    void calculateForces(Components& components);
    void update(Components& components);
    void resolveCollisions(Components& components);

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