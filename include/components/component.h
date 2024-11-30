#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <typeinfo>
#include <typeindex>

#include "config.h"
#include "shape.h"

#include "components/tags.h"
#include "components/physics_components.h"
#include "components/scene_components.h"

using json = nlohmann::ordered_json;

// from_json functions for sfml types
namespace sf {
inline void from_json(const json& j, sf::Vector2f& v) {
    v.x = j.at(0).get<float>();
    v.y = j.at(1).get<float>();
}
inline void from_json(const json& j, sf::Color& c) {
    c.r = j.at(0).get<uint8_t>();
    c.g = j.at(1).get<uint8_t>();
    c.b = j.at(2).get<uint8_t>();
    c.a = j.at(3).get<uint8_t>();
}
}  // namespace sf

namespace twirl {

using EntityName = std::string;
using EntityMap = std::unordered_map<std::string, entt::entity>;

struct DragForceComp {
    // In units of cfg.A
    float drag_coefficient = 0.01f;
    float drag_power = 2.0f;
};
inline void from_json(const json& j, DragForceComp& dfc) {
    dfc.drag_coefficient = j.value("drag_coefficient", 0.01f);
    dfc.drag_power = j.value("drag_power", 2.0f);
}

struct DurabilityComp {
    float durability = 1.0f;
    float durability_loss_per_collision = 0.34f;
    float durability_regen_rate = 0.0f;
    bool delete_at_zero = true;
};
inline void from_json(const json& j, DurabilityComp& dc) {
    dc.durability = j.value("durability", 1.0f);
    dc.durability_loss_per_collision = j.value("durability_loss_per_collision", 0.34f);
    dc.durability_regen_rate = j.value("durability_regen_rate", 0.0f);
    dc.delete_at_zero = j.value("delete_at_zero", true);
}

struct MouseButtonReleasedComp {};

struct PairComp {
    entt::entity target_entity = entt::null;
    entt::entity source_entity = entt::null;
    EntityName target_entity_name;
    EntityName source_entity_name;
};
inline void from_json(const json& j, PairComp& pc) {
    pc.target_entity_name = j.value("target_entity_name", "");
    pc.source_entity_name = j.value("source_entity_name", "");
}

struct PairwiseForceComp {
    float magnitude = -1.0f;  // In units of cfg.A
    float softening = 0.0f;
    float power = 2.0f;
    float min_distance = 0.1f;      // In units of cfg.L
    float distance_scaling = 1.0f;  // in units of cfg.L
};
inline void from_json(const json& j, PairwiseForceComp& pfc) {
    pfc.magnitude = j.value("magnitude", -1.0f);
    pfc.softening = j.value("softening", 0.0f);
    pfc.power = j.value("power", 2.0f);
    pfc.min_distance = j.value("min_distance", 0.1f);
    pfc.distance_scaling = j.value("distance_scaling", 1.0f);
}

struct CollisionComp {};

struct SyncPositionComp {
    bool once_only;
};
inline void from_json(const json& j, SyncPositionComp& spc) {
    spc.once_only = j.value("once_only", true);
}

struct RenderComp {
    TwirlCircleShape shape;
    int zorder = 0;
};
inline void from_json(const json& j, RenderComp& rc) {
    auto radius = j.value("radius", 1.0f) * cfg.L;
    auto outline_thickness = j.value("outline_thickness", 0.0f) * cfg.L;
    auto fill_color = j.value("fill_color", sf::Color::White);
    auto outline_color = j.value("outline_color", sf::Color::Black);
    rc.shape = TwirlCircleShape(radius);
    rc.shape.setOutlineThickness(outline_thickness);
    rc.shape.setFillColor(fill_color);
    rc.shape.setOutlineColor(outline_color);
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
inline void from_json(const json& j, UIComp& uic) {
    uic.pos = j.value("pos", sf::Vector2f(0.0f, 0.0f));
    uic.size = j.value("size", sf::Vector2f(0.0f, 0.0f));
    uic.tracked_value = nullptr;  // Assuming tracked_value is set elsewhere
}

struct StopWatchComp {
    float current_time = 0.0f;
    float end_time = cfg.T;
    bool end_reached = false;
};
inline void from_json(const json& j, StopWatchComp& swc) {
    swc.current_time = j.value("current_time", 0.0f);
    swc.end_time = j.value("end_time", cfg.T);
    swc.end_reached = j.value("end_reached", false);
}

struct DebugComp {};

// This namespace is for component-related functions
namespace comp {

entt::entity getEntityFromCompStr(entt::registry& registry,
                                  const std::string& comp_str);

}

}  // namespace twirl

#endif  // COMPONENT_H