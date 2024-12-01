#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include "components/components.h"
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

namespace comp {

void emplaceComponent(entt::registry& registry, entt::entity entity,
                      const std::string& comp_key, const json& comp_json) {
    if (comp_key == "EnemyComp") {
        registry.emplace<EnemyComp>(entity);
    } else if (comp_key == "PhysicsComp") {
        auto physicscomp = comp_json.template get<PhysicsComp>();
        registry.emplace<PhysicsComp>(entity, physicscomp);
    } else {
        throw std::runtime_error("Unknown component type");
    }
}

entt::entity getEntityFromStr(entt::registry& registry, const std::string& input_str) {
    // Parse the input string
    size_t colon = input_str.find(':');
    if (colon == std::string::npos) {
        throw std::runtime_error("Colon not found in component string");
    }
    std::string comp_str = input_str.substr(0, colon);
    std::string selection_str = input_str.substr(colon + 1);

    // Get the view based on the component string
    if (comp_str == "EnemyComp") {
        return getEntityFromSelectionStr<EnemyComp>(registry, selection_str);
    } else if (comp_str == "PhysicsComp") {
        return getEntityFromSelectionStr<PhysicsComp>(registry, selection_str);
    } else {
        throw std::runtime_error("Unknown component type");
    }
}

}  // namespace comp
}  // namespace twirl