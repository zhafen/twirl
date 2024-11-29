#include "components/component.h"

#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>

namespace twirl {

// Define a type map
std::unordered_map<std::string, std::type_index> compTypeMap = {
    {"EnemyComp", std::type_index(typeid(EnemyComp))},
    {"DebugComp", std::type_index(typeid(DebugComp))},
};

namespace comp {

entt::entity getEntityFromCompStr(entt::registry& registry,
                                  const std::string& comp_str) {

    // Define a type map
    std::unordered_map<std::string, std::type_index> typeMap = {
        {"EnemyComp", std::type_index(typeid(EnemyComp))},
        {"DebugComp", std::type_index(typeid(DebugComp))},
    };

    std::type_index comp_type = typeMap.at(comp_str);
    auto rview = registry.view<comp_type>();

    return entt::null;
}

}  // namespace comp

}  // namespace twirl