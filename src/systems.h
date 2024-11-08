#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include "game_objects.h"

using EntityId = int;
using EntityRelationId = int;

struct Components;

struct MetadataComponent {
    EntityId id;
    std::string name;
};

struct RenderComponent {
    std::shared_ptr<sf::Shape> shape;
    int zorder = 0;
    sf::Vector2f pos;
};

// All UI components are assumed to be rectangles that track floats.
// If I start using substructures, I may need to change ui_comps to holding pointers.
struct UIComponent : RenderComponent {
    float& tracked_value;
    sf::Vector2f size;
    UIComponent(float& tracked_value) : tracked_value(tracked_value) {}
};

struct PhysicsComponent {
    float mass = 1.0f;
    sf::Vector2f pos = {0.0f, 0.0f};
    sf::Vector2f vel = {0.0f, 0.0f};
    sf::Vector2f force = {0.0f, 0.0f};
    bool collided = false;
};

struct DurabilityComponent {
    float durability = 1.0f;
    float durability_loss_per_collision = 0.34f;
    float durability_regen_rate = 0.1f;
    bool lost_all_durability = false;
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

struct PairwiseComponent {
    EntityId id1;
    EntityId id2;
};

struct CollisionComponent : PairwiseComponent {};

// Very general component for applying a function to pairs of entities
struct PairwiseFunctionComponent : PairwiseComponent {
    std::function<void(EntityId id1, EntityId id2, Components& components)> func;
};

struct Components {
    // Single-entity components
    std::unordered_map<EntityId, MetadataComponent> metadata_comps;
    std::unordered_map<EntityId, RenderComponent> render_comps;
    std::unordered_map<EntityId, UIComponent> ui_comps;
    std::unordered_map<EntityId, PhysicsComponent> physics_comps;
    std::unordered_map<EntityId, DurabilityComponent> dura_comps;
    std::unordered_map<EntityId, MouseButtonReleasedComponent>
        mousebuttonreleased_comps;
    std::vector<std::pair<int, EntityId>> entity_zorders;
    std::vector<std::pair<int, EntityId>> ui_entity_zorders;

    // Multi-entity components
    std::unordered_map<EntityRelationId, PairwiseForceComponent> pairforce_comps;
    std::unordered_map<EntityRelationId, CollisionComponent> collision_comps;
    std::unordered_map<EntityRelationId, PairwiseFunctionComponent> pairfunc_comps;
};

// Probably best to avoid using the general system when possible
class GeneralSystem {
   public:
    GeneralSystem(const Config& cfg);
    void callPairwiseFunctions(Components& components);

   private:
    Config cfg;
};

class PhysicsSystem {
   public:
    PhysicsSystem(const Config& cfg);
    void calculateForces(Components& components);
    void update(Components& components);
    void resolveCollisions(Components& components);
    void updateDurability(Components& components);

   private:
    Config cfg;
};

class RenderSystem {
   public:
    RenderSystem(const Config& cfg, sf::View& view, sf::View& ui_view);
    void render(EntityId player_id, sf::RenderWindow& window, Components& components);
    void renderUI(sf::RenderWindow& window, Components& components);

   private:
    sf::View view;
    sf::View ui_view;
    Config cfg;
};

#endif  // SYSTEMS_H