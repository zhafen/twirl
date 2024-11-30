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
#include "components/base_components.h"
#include "components/ui_components.h"

namespace twirl {

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