#ifndef SYSTEM_COMPONENTS_H
#define SYSTEM_COMPONENTS_H

#include <SFML/Graphics.hpp>
#include "components/base_components.h"
#include "shape.h"
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct SceneComp {
    std::string scene_fp;
    bool emplace_after_loading = false;
    int emplace_order = 0;
    json json_data;
    size_t n_emplaced = 0;
};
inline void from_json(const json& j, SceneComp& scenecomp) {
    scenecomp.scene_fp = j.at("scene_fp").get<std::string>();
    scenecomp.emplace_after_loading = j.value("emplace_after_loading", false);
    scenecomp.emplace_order = j.value("emplace_order", 0);
}

struct TriggerComp {
    int n_triggers = 0;
};
inline void from_json(const json& j, TriggerComp& triggercomp) {
    triggercomp.n_triggers = j.value("n_triggers", 0);
}

struct PairComp {
    entt::entity target_entity = entt::null;
    entt::entity source_entity = entt::null;
    EntityName target_entity_name = "";
    EntityName source_entity_name = "";
};
inline void from_json(const json& j, PairComp& paircomp) {
    paircomp.target_entity_name = j.value("target_entity_name", "");
    paircomp.source_entity_name = j.value("source_entity_name", "");
}

struct SyncPositionComp {
    bool once_only = false;
};
inline void from_json(const json& j, SyncPositionComp& syncpositioncomp) {
    syncpositioncomp.once_only = j.value("once_only", false);
}

struct WatchComp {
    float current_time = 0.0f;
    float end_time = 1.0f * cfg.T;
    bool end_reached = false;
};
inline void from_json(const json& j, WatchComp& watchcomp) {
    watchcomp.current_time = j.value("current_time", 0.0f);
    watchcomp.end_time = j.value("end_time", 1.0f) * cfg.T;
    watchcomp.end_reached = j.value("end_reached", false);
}

struct PhysicsComp {
    float mass = 1.0f;
    sf::Vector2f pos = sf::Vector2f(0.0f, 0.0f) * cfg.L;
    sf::Vector2f vel = sf::Vector2f(0.0f, 0.0f) * cfg.V;
    sf::Vector2f force = sf::Vector2f(0.0f, 0.0f) * cfg.A;
    float internal_energy = 0.0f * cfg.E;
    bool collided = false;
};
inline void from_json(const json& j, PhysicsComp& physicscomp) {
    physicscomp.mass = j.value("mass", 1.0f);
    physicscomp.pos = j.value("pos", sf::Vector2f(0.0f, 0.0f)) * cfg.L;
    physicscomp.vel = j.value("vel", sf::Vector2f(0.0f, 0.0f)) * cfg.V;
    physicscomp.internal_energy = j.value("internal_energy", 0.0f) * cfg.E;
}

struct DragForceComp {
    float drag_coefficient = 0.01f * cfg.A;
    float drag_power = 2.0f;
};
inline void from_json(const json& j, DragForceComp& dragforcecomp) {
    dragforcecomp.drag_coefficient = j.value("drag_coefficient", 0.01f) * cfg.A;
    dragforcecomp.drag_power = j.value("drag_power", 2.0f);
}

struct DurabilityComp {
    float durability;
    float durability_frac = 1.0f;
    float max_durability = 1.0f * cfg.E;
    float energy_to_durability = 1.0f;
    float durability_regen_rate = 0.0f * cfg.E / cfg.T;
    float delete_at_zero = true;
};
inline void from_json(const json& j, DurabilityComp& durabilitycomp) {
    durabilitycomp.durability_frac = j.value("durability_frac", 1.0f);
    durabilitycomp.max_durability = j.value("max_durability", 1.0f) * cfg.E;
    durabilitycomp.energy_to_durability = j.value("energy_to_durability", 1.0f);
    durabilitycomp.durability_regen_rate = j.value("durability_regen_rate", 0.0f) * cfg.E / cfg.T;
    durabilitycomp.delete_at_zero = j.value("delete_at_zero", true);
    durabilitycomp.durability = durabilitycomp.durability_frac * durabilitycomp.max_durability;
}

struct PairwiseForceComp {
    float magnitude = -1.0f * cfg.A;
    float softening = 0.0f * cfg.L;
    float power = 2.0f;
    float min_distance = 0.1f * cfg.L;
    float distance_scaling = 1.0f * cfg.H;
};
inline void from_json(const json& j, PairwiseForceComp& pairwiseforcecomp) {
    pairwiseforcecomp.magnitude = j.value("magnitude", -1.0f) * cfg.A;
    pairwiseforcecomp.softening = j.value("softening", 0.0f) * cfg.L;
    pairwiseforcecomp.power = j.value("power", 2.0f);
    pairwiseforcecomp.min_distance = j.value("min_distance", 0.1f) * cfg.L;
    pairwiseforcecomp.distance_scaling = j.value("distance_scaling", 1.0f) * cfg.H;
}

struct CollisionComp {
    float fraction_energy_converted = 0.1f;
};
inline void from_json(const json& j, CollisionComp& collisioncomp) {
    collisioncomp.fraction_energy_converted = j.value("fraction_energy_converted", 0.1f);
}

struct MouseButtonReleasedComp {};

struct CircleComp {
    CenteredCircleShape shape;
    float radius = 1.0f * cfg.L;
    float outline_thickness = 0.0f * cfg.L;
    sf::Color fill_color = sf::Color::White;
    sf::Color outline_color = sf::Color::Black;
};
inline void from_json(const json& j, CircleComp& circlecomp) {
    circlecomp.radius = j.value("radius", 1.0f) * cfg.L;
    circlecomp.outline_thickness = j.value("outline_thickness", 0.0f) * cfg.L;
    circlecomp.fill_color = j.value("fill_color", sf::Color::White);
    circlecomp.outline_color = j.value("outline_color", sf::Color::Black);
    circlecomp.shape = CenteredCircleShape(circlecomp.radius);
    circlecomp.shape.setOutlineThickness(circlecomp.outline_thickness);
    circlecomp.shape.setFillColor(circlecomp.fill_color);
    circlecomp.shape.setOutlineColor(circlecomp.outline_color);
}

struct RectangleComp {
    sf::RectangleShape shape;
    sf::Vector2f size = sf::Vector2f(1.0f, 1.0f) * cfg.L;
    sf::Vector2f pos = sf::Vector2f(0.0f, 0.0f) * cfg.L;
    float outline_thickness = 0.0f * cfg.L;
    sf::Color fill_color = sf::Color::White;
    sf::Color outline_color = sf::Color::Black;
};
inline void from_json(const json& j, RectangleComp& rectanglecomp) {
    rectanglecomp.size = j.value("size", sf::Vector2f(1.0f, 1.0f)) * cfg.L;
    rectanglecomp.pos = j.value("pos", sf::Vector2f(0.0f, 0.0f)) * cfg.L;
    rectanglecomp.outline_thickness = j.value("outline_thickness", 0.0f) * cfg.L;
    rectanglecomp.fill_color = j.value("fill_color", sf::Color::White);
    rectanglecomp.outline_color = j.value("outline_color", sf::Color::Black);
    rectanglecomp.shape.setSize(rectanglecomp.size);
    rectanglecomp.shape.setPosition(rectanglecomp.pos);
    rectanglecomp.shape.setOutlineThickness(rectanglecomp.outline_thickness);
    rectanglecomp.shape.setFillColor(rectanglecomp.fill_color);
    rectanglecomp.shape.setOutlineColor(rectanglecomp.outline_color);
}

struct ValueBarComp {
    sf::RectangleShape shape;
    sf::Vector2f pos = sf::Vector2f(-50.f, -45.f) * cfg.L;
    sf::Vector2f size = sf::Vector2f(100.f, 1.0f) * cfg.L;
    float* tracked_value = nullptr;
    EntityName tracked_entity_name = "";
};
inline void from_json(const json& j, ValueBarComp& valuebarcomp) {
    valuebarcomp.pos = j.value("pos", sf::Vector2f(-50.f, -45.f)) * cfg.L;
    valuebarcomp.size = j.value("size", sf::Vector2f(100.f, 1.0f)) * cfg.L;
    valuebarcomp.tracked_entity_name = j.value("tracked_entity_name", "");
}

struct ZOrderComp {
    int zorder = 0;
};
inline void from_json(const json& j, ZOrderComp& zordercomp) {
    zordercomp.zorder = j.value("zorder", 0);
}

struct TextComp {
    CenteredText text;
    std::string string = "[placeholder text]";
    sf::Vector2f pos = sf::Vector2f(0.f, -25.f) * cfg.L;
    sf::Vector2f origin = sf::Vector2f(0.5f, 0.5f);
    unsigned int character_size = 24;
};
inline void from_json(const json& j, TextComp& textcomp) {
    textcomp.string = j.value("string", "[placeholder text]");
    textcomp.pos = j.value("pos", sf::Vector2f(0.f, -25.f)) * cfg.L;
    textcomp.origin = j.value("origin", sf::Vector2f(0.5f, 0.5f));
    textcomp.character_size = j.value("character_size", 24);
    textcomp.text.setString(textcomp.string);
    textcomp.text.setPosition(textcomp.pos);
    textcomp.text.setCharacterSize(textcomp.character_size);
    // Origin is set relative to the dimensions of the text
    sf::FloatRect text_rect = textcomp.text.getLocalBounds();
    textcomp.text.setOrigin(
        text_rect.left + text_rect.width * textcomp.origin.x,
        text_rect.top + text_rect.height * textcomp.origin.y);
}

struct DebugComp {
    bool verbose = true;
};
inline void from_json(const json& j, DebugComp& debugcomp) {
    debugcomp.verbose = j.value("verbose", true);
}

}  // namespace twirl

#endif  // SYSTEM_COMPONENTS_H