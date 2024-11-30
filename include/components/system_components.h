#ifndef SYSTEM_COMPONENTS_H
#define SYSTEM_COMPONENTS_H

#include <SFML/Graphics.hpp>
#include "components/base_components.h"
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct SceneComp {
    std::string scene_fp;
    bool emplace_after_loading = false;
    json json_data;
    size_t n_emplaced = 0;
    bool verbose_names = true;
};
inline void from_json(const json& j, SceneComp& scenecomp) {
    scenecomp.scene_fp = j.at("scene_fp").get<std::string>();
    scenecomp.emplace_after_loading = j.value("emplace_after_loading", false);
    scenecomp.verbose_names = j.value("verbose_names", true);
}

struct SceneTriggerComp {
    std::string scene_name;
    entt::entity scene_entity = entt::null;
    int n_triggers = 0;
};
inline void from_json(const json& j, SceneTriggerComp& scenetriggercomp) {
    scenetriggercomp.scene_name = j.at("scene_name").get<std::string>();
    scenetriggercomp.n_triggers = j.value("n_triggers", 0);
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

struct PairwiseForceComp {
    float magnitude = -1.0f;
    float softening = 0.0f;
    float power = 2.0f;
    float min_distance = 0.0f;
    float distance_scaling = 1.0f;
};
inline void from_json(const json& j, PairwiseForceComp& pairwiseforcecomp) {
    pairwiseforcecomp.magnitude = j.value("magnitude", -1.0f);
    pairwiseforcecomp.softening = j.value("softening", 0.0f);
    pairwiseforcecomp.power = j.value("power", 2.0f);
    pairwiseforcecomp.min_distance = j.value("min_distance", 0.0f);
    pairwiseforcecomp.distance_scaling = j.value("distance_scaling", 1.0f);
}

struct CollisionComp {};

struct MouseButtonReleasedComp {};

struct RenderComp {
    TwirlCircleShape shape;
    int zorder = 0;
};
inline void from_json(const json& j, RenderComp& rendercomp) {
    rendercomp.zorder = j.value("zorder", 0);
    auto radius = j.value("radius", 1.0f) * cfg.L;
    auto outline_thickness = j.value("outline_thickness", 0.0f) * cfg.L;
    auto fill_color = j.value("fill_color", sf::Color::White);
    auto outline_color = j.value("outline_color", sf::Color::Black);
    rendercomp.shape = TwirlCircleShape(radius);
    rendercomp.shape.setOutlineThickness(outline_thickness);
    rendercomp.shape.setFillColor(fill_color);
    rendercomp.shape.setOutlineColor(outline_color);
}

}  // namespace twirl

#endif  // SYSTEM_COMPONENTS_H