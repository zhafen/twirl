#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics.hpp>
#include <string>

#include "config.h"
#include "shape.h"

namespace cc {

struct PhysicsComp {
    float mass = 1.0f;
    sf::Vector2f pos = {0.0f, 0.0f};
    sf::Vector2f vel = {0.0f, 0.0f};
    sf::Vector2f force = {0.0f, 0.0f};
    bool collided = false;
};

struct ForceComp {
    // In units of cfg.A / cfg.V
    float drag_coefficient = 0.01f;
    float drag_power = 2.0f;
};

struct DurabilityComp {
    float durability = 1.0f;
    float durability_loss_per_collision = 0.34f;
    float durability_regen_rate = 0.1f;
};

struct MouseButtonReleasedComp {};

struct PairComp {
    entt::entity target;
    entt::entity source;
};

struct PairwiseForceComp {
    float magnitude = 1.0f;  // In units of cfg.A
    float softening = 0.0f;
    float power = 2.0f;
    float min_distance = 0.1f;      // In units of cfg.L
    float distance_scaling = 1.0f;  // in units of cfg.L
};

struct CollisionComp {};

// Very general component for applying a function to pairs of entities
struct PairwiseFunctionComp {
    // std::function<void(EntityId id1, EntityId id2, Comps& components)> func;
};

struct RenderComp {
    CCCircleShape shape;
    int zorder = 0;
};

// All UI components are assumed to be rectangles that track floats.
// If I start using substructures, I may need to change ui_comps to holding pointers.
struct UIComp {
    sf::RectangleShape shape;
    float& tracked_value;
    sf::Vector2f size;
    UIComp(float& tracked_value) : tracked_value(tracked_value) {}
};

// struct CompsOld {
//     // Single-entity components
//     std::unordered_map<EntityId, RenderComp> render_comps;
//     std::unordered_map<EntityId, UIComp> ui_comps;
//     std::unordered_map<EntityId, PhysicsComp> physics_comps;
//     std::unordered_map<EntityId, ForceComp> force_comps;
//     std::unordered_map<EntityId, DurabilityComp> dura_comps;
//     std::unordered_map<EntityId, MouseButtonReleasedComp>
//         mousebuttonreleased_comps;
//     std::vector<std::pair<int, EntityId>> rc_zorders;
//     std::vector<std::pair<int, EntityId>> uic_zorders;
// 
//     // Multi-entity components
//     std::unordered_map<EntityRelationId, PairwiseForceComp> pairforce_comps;
//     std::unordered_map<EntityRelationId, CollisionComp> collision_comps;
//     std::unordered_map<EntityRelationId, PairwiseFunctionComp> pairfunc_comps;
// };

}  // namespace cc

#endif  // COMPONENT_H