#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/view.hpp>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <typeindex>
#include <typeinfo>

#include "components/base_components.h"
#include "components/system_components.h"
#include "components/tags.h"
#include "config.h"
#include "shape.h"

namespace twirl {

namespace comp {

entt::entity getEntityFromStrWrapper(entt::registry& registry,
                                     const std::string& comp_str);

template <typename Type>
[[nodiscard]] entt::entity getEntityFromStr(entt::registry& registry,
                                            const std::string& selection_str) {

    // Get the view
    auto rview = registry.view<Type>();

    // Get the entity from the view based on the selection string
    if (selection_str == "0") {
        return rview.front();
    } else {
        throw std::runtime_error("Selection string not recognized");
    }
}

void emplaceComponent(entt::registry& registry, entt::entity entity,
                      const std::string& comp_key, const json& comp_json);

}  // namespace comp

}  // namespace twirl

#endif  // COMPONENT_H