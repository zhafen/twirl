#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <SFML/Graphics.hpp>

#include "config.h"

namespace cc {

class Components;

class Component {};

class MetadataComponent : Component {
    public:
    EntityId id;
    std::string name;
};

class PhysicsComponent : Component {
    public:
    float mass = 1.0f;
    sf::Vector2f pos = {0.0f, 0.0f};
    sf::Vector2f vel = {0.0f, 0.0f};
    sf::Vector2f force = {0.0f, 0.0f};
    bool collided = false;
};

class ForceComponent : Component {
    public:
    // In units of cfg.A / cfg.V
    float drag_coefficient = 0.01f;
    float drag_power = 2.0f;
};

class DurabilityComponent : Component{
    float durability = 1.0f;
    float durability_loss_per_collision = 0.34f;
    float durability_regen_rate = 0.1f;
};

class MouseButtonReleasedComponent : Component {};

class PairwiseForceComponent : Component {
    EntityId target_entity;
    EntityId source_entity;
    class Parameters {
        float magnitude = 1.0f;  // In units of cfg.A
        float softening = 0.0f;
        float power = 2.0f;
        float min_distance = 0.1f;      // In units of cfg.L
        float distance_scaling = 1.0f;  // in units of cfg.L
    } params;
};

class PairwiseComponent : Component{
    EntityId id1;
    EntityId id2;
};

class CollisionComponent : PairwiseComponent {};

// Very general component for applying a function to pairs of entities
class PairwiseFunctionComponent : PairwiseComponent {
    std::function<void(EntityId id1, EntityId id2, Components& components)> func;
};

class RenderComponent : Component {
    std::shared_ptr<sf::Shape> shape;
    int zorder = 0;
    sf::Vector2f pos;
};

// All UI components are assumed to be rectangles that track floats.
// If I start using subclassures, I may need to change ui_comps to holding pointers.
class UIComponent : RenderComponent {
    float& tracked_value;
    sf::Vector2f size;
    UIComponent(float& tracked_value) : tracked_value(tracked_value) {}
};

class Components {
    // Single-entity components
    std::unordered_map<int, std::unordered_map<EntityId, Component>> components;

    // Ordering
    std::vector<std::pair<int, EntityId>> entity_zorders;
    std::vector<std::pair<int, EntityId>> ui_entity_zorders;

    // Multi-entity components
    std::unordered_map<EntityRelationId, PairwiseForceComponent> pairforce_comps;
    std::unordered_map<EntityRelationId, CollisionComponent> collision_comps;
    std::unordered_map<EntityRelationId, PairwiseFunctionComponent> pairfunc_comps;
};

} // namespace cc

#endif  // COMPONENT_H