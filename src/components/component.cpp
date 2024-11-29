#include "components/component.h"

#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>

namespace twirl {

// Define a type map
std::unordered_map<std::string, std::type_index> comp_map = {
    {"EnemyComp", std::type_index(typeid(EnemyComp))},
    {"DebugComp", std::type_index(typeid(DebugComp))},
};

namespace comp {

entt::entity getEntityFromCompStr(entt::registry& registry,
                                  const std::string& comp_str) {

    // return registry.view<comp_map[comp_str]>().front();

    return entt::null;
}

}  // namespace comp

}  // namespace twirl