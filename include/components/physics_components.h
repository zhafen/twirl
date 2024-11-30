#ifndef PHYSICS_COMPONENTS_H
#define PHYSICS_COMPONENTS_H

#include <SFML/Graphics.hpp>
#include "components/sfml_components.h"
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct PhysicsComp {
    float mass = 1.0f;
    sf::Vector2f pos = sf::Vector2f(0.0f, 0.0f) * cfg.L;
    sf::Vector2f vel = sf::Vector2f(0.0f, 0.0f) * cfg.V;
    sf::Vector2f force = sf::Vector2f(0.0f, 0.0f) * cfg.A;
    bool collided = false;
};
inline void from_json(const json& j, PhysicsComp& physicscomp) {
    physicscomp.mass = j.value("mass", 1.0f);
    physicscomp.pos = j.value("pos", sf::Vector2f(0.0f, 0.0f)) * cfg.L;
    physicscomp.vel = j.value("vel", sf::Vector2f(0.0f, 0.0f)) * cfg.V;
}

struct DragForceComp {
    float drag_coefficient = 0.01f;
    float drag_power = 2.0f;
};
inline void from_json(const json& j, DragForceComp& dragforcecomp) {
    dragforcecomp.drag_coefficient = j.value("drag_coefficient", 0.01f);
    dragforcecomp.drag_power = j.value("drag_power", 2.0f);
}

struct DurabilityComp {
    float durability = 1.0f;
    float durability_loss_per_collision = 0.34f;
    float durability_regen_rate = 0.0f;
    float delete_at_zero = true;
};
inline void from_json(const json& j, DurabilityComp& durabilitycomp) {
    durabilitycomp.durability = j.value("durability", 1.0f);
    durabilitycomp.durability_loss_per_collision = j.value("durability_loss_per_collision", 0.34f);
    durabilitycomp.durability_regen_rate = j.value("durability_regen_rate", 0.0f);
    durabilitycomp.delete_at_zero = j.value("delete_at_zero", true);
}

}  // namespace twirl

#endif  // PHYSICS_COMPONENTS_H