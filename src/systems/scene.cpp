#include "systems/scene.h"

#include <entt/entity/helper.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "components/components.h"

using json = nlohmann::ordered_json;

namespace twirl {

void SceneSystem::loadJsonData(entt::registry& registry) {
    for (auto [entity, scene_c] : registry.view<SceneComp>().each()) {
        // Parse the file
        std::ifstream file(scene_c.scene_fp);
        if (!file.is_open()) {
            throw std::runtime_error(
                "Failed to open file: " + scene_c.scene_fp +
                "\nCurrent directory: " + std::filesystem::current_path().string());
            return;
        }
        scene_c.json_data = json::parse(file);

        if (scene_c.emplace_after_loading) {
            emplaceScene(registry, entity);
        }
    }
}

void SceneSystem::onSceneTrigger(entt::registry& registry, entt::entity entity) {
    auto& scenetrigger_c = registry.get<SceneTriggerComp>(entity);
    emplaceScene(registry, scenetrigger_c.scene_entity);

    // Reset n_triggers
    scenetrigger_c.n_triggers = 0;
}

void SceneSystem::emplaceScene(entt::registry& registry,
                               const entt::entity scene_entity) {
    auto& scene_c = registry.get<SceneComp>(scene_entity);
    auto scene_name = registry.get<EntityName>(scene_entity);

    std::cout << "Emplacing scene: " << scene_c.scene_fp << std::endl;

    // Loop through and emplace entities
    EntityMap scene_entity_map;
    for (auto [entity_name_base, entity_json] : scene_c.json_data.items()) {
        EntityName entity_name;
        if (scene_c.verbose_names) {
            entity_name = scene_name + "." + std::to_string(scene_c.n_emplaced) + "." +
                          entity_name_base;
        } else {
            entity_name = entity_name_base;
        }
        scene_entity_map[entity_name] =
            emplaceEntity(registry, entity_name, entity_json);
    }

    // Resolve names
    auto rview = registry.view<UnresolvedNameComp>();
    for (auto entity : rview) {
        // DEBUG
        auto this_entity_name = registry.get<EntityName>(entity);
        // Check for each component that could have an unresolved name
        auto* pair_c_ptr = registry.try_get<PairComp>(entity);
        if (pair_c_ptr != nullptr) {
            pair_c_ptr->target_entity = resolveEntityName(
                registry, scene_entity_map, pair_c_ptr->target_entity_name);
            pair_c_ptr->source_entity = resolveEntityName(
                registry, scene_entity_map, pair_c_ptr->source_entity_name);
        }
        auto* scenetrigger_c_ptr = registry.try_get<SceneTriggerComp>(entity);
        if (scenetrigger_c_ptr != nullptr) {
            scenetrigger_c_ptr->scene_entity = resolveEntityName(
                registry, scene_entity_map, scenetrigger_c_ptr->scene_name);
        }
        registry.remove<UnresolvedNameComp>(entity);
    }

    // Increment n_emplaced
    scene_c.n_emplaced++;
}

entt::entity SceneSystem::emplaceEntity(entt::registry& registry,
                                        const EntityName entity_name,
                                        const json& entity_json) {
    // Create an entity and store its name and ID
    auto entity = registry.create();
    registry.emplace<EntityName>(entity, entity_name);

    // just one of the components is failing
    json components = entity_json["components"];
    for (const auto& [comp_key, comp_json] : components.items()) {
        comp::emplaceComponent(registry, entity, comp_key, comp_json);
    }

    return entity;
}

entt::entity SceneSystem::resolveEntityName(entt::registry& registry,
                                            EntityMap entity_map,
                                            EntityName entity_name) {
    // Parse as needed
    if (entity_name.front() == '[' && entity_name.back() == ']') {
        std::string input_str = entity_name.substr(1, entity_name.size() - 2);
        return comp::getEntityFromStr(registry, input_str);
    }

    // Otherwise return simply the name for the map
    return entity_map.at(entity_name);
}

}  // namespace twirl