#ifndef COMPONENT_H
#define COMPONENT_H

#include <SFML/Graphics.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
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

entt::entity getEntityFromStr(entt::registry& registry,
                                  const std::string& comp_str);

entt::view<EnemyComp> getViewFromStr(entt::registry& registry, const std::string& view_str);

void emplaceComponent(entt::registry& registry, entt::entity entity,
                      const std::string& comp_key, const json& comp_json);

}  // namespace comp

}  // namespace twirl

#endif  // COMPONENT_H