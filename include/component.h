#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics.hpp>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <string>

#include "config.h"
#include "shape.h"

// from_json functions for sfml types
namespace sf {
inline void from_json(const nlohmann::json& j, sf::Vector2f& v) {
    v.x = j.at(0).get<float>();
    v.y = j.at(1).get<float>();
}
inline void from_json(const nlohmann::json& j, sf::Color& c) {
    c.r = j.at(0).get<uint8_t>();
    c.g = j.at(1).get<uint8_t>();
    c.b = j.at(2).get<uint8_t>();
    c.a = j.at(3).get<uint8_t>();
}
}  // namespace sf

namespace twirl {

// We don't define a macro for MetadataComp because currently the key for a given
// entity in the json is the name.
struct MetadataComp {
    std::string name;
};

struct EnemyComp {};

struct DeleteComp {};

struct SpawnComp {};

struct PhysicsComp {
    float mass = 1.0f;
    sf::Vector2f pos = {0.0f, 0.0f};
    sf::Vector2f vel = {0.0f, 0.0f};
    sf::Vector2f force = {0.0f, 0.0f};
    bool collided = false;
};
// Have to define this without a macro because of the vector types
inline void from_json(const nlohmann::json& j, PhysicsComp& pc) {
    pc.mass = j.value("mass", 1.0f);
    pc.pos = j.value("pos", sf::Vector2f(0.0f, 0.0f));
    pc.vel = j.value("vel", sf::Vector2f(0.0f, 0.0f));
    pc.force = j.value("force", sf::Vector2f(0.0f, 0.0f));
    pc.collided = j.value("collided", false);
}

struct DragForceComp {
    // In units of cfg.A
    float drag_coefficient = 0.01f;
    float drag_power = 2.0f;
};
inline void from_json(const nlohmann::json& j, DragForceComp& dfc) {
    dfc.drag_coefficient = j.value("drag_coefficient", 0.01f);
    dfc.drag_power = j.value("drag_power", 2.0f);
}

struct DurabilityComp {
    float durability = 1.0f;
    float durability_loss_per_collision = 0.34f;
    float durability_regen_rate = 0.0f;
    bool delete_at_zero = true;
};
inline void from_json(const nlohmann::json& j, DurabilityComp& dc) {
    dc.durability = j.value("durability", 1.0f);
    dc.durability_loss_per_collision = j.value("durability_loss_per_collision", 0.34f);
    dc.durability_regen_rate = j.value("durability_regen_rate", 0.0f);
    dc.delete_at_zero = j.value("delete_at_zero", true);
}

struct MouseButtonReleasedComp {};

struct PairwiseForceComp {
    entt::entity target_entity;
    entt::entity source_entity;
    struct Params {
        float magnitude = -1.0f;  // In units of cfg.A
        float softening = 0.0f;
        float power = 2.0f;
        float min_distance = 0.1f;      // In units of cfg.L
        float distance_scaling = 1.0f;  // in units of cfg.L
    } params;
};

struct CollisionComp {
    entt::entity entity1;
    entt::entity entity2;
};

// Very general component for applying a function to pairs of entities
struct PairwiseFunctionComp {
    // std::function<void(EntityId entity1, EntityId entity2, Comps& components)> func;
};

struct RenderComp {
    TwirlCircleShape shape;
    int zorder = 0;
};
inline void from_json(const nlohmann::json& j, RenderComp& rc) {
    auto radius = j.value("radius", 1.0f) * cfg.L;
    auto fill_color = j.value("fill_color", sf::Color::White);
    rc.shape = TwirlCircleShape(radius);
    rc.shape.setFillColor(fill_color);
    rc.zorder = j.value("zorder", 0);
}

// All UI components are assumed to be rectangles that track floats.
// If I start using substructures, I may need to change ui_comps to holding pointers.
struct UIComp {
    sf::RectangleShape shape;

    sf::Vector2f pos;
    sf::Vector2f size;

    // sf::RectangleShape shape;

    float* tracked_value;
};

struct StopWatchComp {
    float current_time = 0.0f;
    float end_time = 1.0f;
    bool end_reached = false;
};

}  // namespace twirl

#endif  // COMPONENT_H