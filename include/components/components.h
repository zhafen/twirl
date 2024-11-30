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
#include "components/system_components.h"
#include "components/base_components.h"

namespace twirl {

// This namespace is for component-related functions
namespace comp {

entt::entity getEntityFromCompStr(entt::registry& registry,
                                  const std::string& comp_str);

void emplaceComponent(entt::registry& registry, entt::entity entity,
                      const std::string& comp_key, const json& comp_json);

}

}  // namespace twirl

#endif  // COMPONENT_H