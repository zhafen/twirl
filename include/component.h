#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics.hpp>
#include <string>

#include "config.h"
#include "shape.h"

namespace cc {

struct Component {};

struct MetadataComponent {
    std::string name;
};

struct PhysicsComponent {
    float mass = 1.0f;
    sf::Vector2f pos = {0.0f, 0.0f};
    sf::Vector2f vel = {0.0f, 0.0f};
    sf::Vector2f force = {0.0f, 0.0f};
    bool collided = false;
};

struct ForceComponent {
    // In units of cfg.A / cfg.V
    float drag_coefficient = 0.01f;
    float drag_power = 2.0f;
};

struct DurabilityComponent {
    float durability = 1.0f;
    float durability_loss_per_collision = 0.34f;
    float durability_regen_rate = 0.1f;
};

struct MouseButtonReleasedComponent {};

struct PairwiseForceComponent {
    EntityId target_entity;
    EntityId source_entity;
    struct Parameters {
        float magnitude = 1.0f;  // In units of cfg.A
        float softening = 0.0f;
        float power = 2.0f;
        float min_distance = 0.1f;      // In units of cfg.L
        float distance_scaling = 1.0f;  // in units of cfg.L
    } params;
};

struct PairwiseComponent {
    EntityId id1;
    EntityId id2;
};

struct CollisionComponent : PairwiseComponent {};

// Very general component for applying a function to pairs of entities
struct PairwiseFunctionComponent : PairwiseComponent {
    // std::function<void(EntityId id1, EntityId id2, Components& components)> func;
};

struct RenderComponent {
    CCCircleShape shape;
    int zorder = 0;
};

// All UI components are assumed to be rectangles that track floats.
// If I start using substructures, I may need to change ui_comps to holding pointers.
struct UIComponent : RenderComponent {
    float& tracked_value;
    sf::Vector2f size;
    UIComponent(float& tracked_value) : tracked_value(tracked_value) {}
};

// struct ComponentsOld {
//     // Single-entity components
//     std::unordered_map<EntityId, MetadataComponent> metadata_comps;
//     std::unordered_map<EntityId, RenderComponent> render_comps;
//     std::unordered_map<EntityId, UIComponent> ui_comps;
//     std::unordered_map<EntityId, PhysicsComponent> physics_comps;
//     std::unordered_map<EntityId, ForceComponent> force_comps;
//     std::unordered_map<EntityId, DurabilityComponent> dura_comps;
//     std::unordered_map<EntityId, MouseButtonReleasedComponent>
//         mousebuttonreleased_comps;
//     std::vector<std::pair<int, EntityId>> entity_zorders;
//     std::vector<std::pair<int, EntityId>> ui_entity_zorders;
// 
//     // Multi-entity components
//     std::unordered_map<EntityRelationId, PairwiseForceComponent> pairforce_comps;
//     std::unordered_map<EntityRelationId, CollisionComponent> collision_comps;
//     std::unordered_map<EntityRelationId, PairwiseFunctionComponent> pairfunc_comps;
// };

}  // namespace cc

#endif  // COMPONENT_H