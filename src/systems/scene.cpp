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

bool SceneSystem::checkSceneTriggers(entt::registry& registry) {
    bool any_scene_triggered = false;

    // Triggers caused by a timer running out
    any_scene_triggered =
        any_scene_triggered |
        checkSceneTriggersForFlag<WatchTriggerFlag>(
            registry, [](entt::registry& registry, entt::entity entity) -> bool {
                return registry.get<WatchComp>(entity).end_reached;
            });

    // Triggers caused by durability reaching 0
    any_scene_triggered =
        any_scene_triggered |
        checkSceneTriggersForFlag<DurabilityTriggerFlag>(
            registry, [](entt::registry& registry, entt::entity entity) -> bool {
                return registry.get<DurabilityComp>(entity).durability <= 0;
            });

    // Triggers caused by no enemies existing
    any_scene_triggered =
        any_scene_triggered |
        checkSceneTriggersForFlag<EnemyAbsenceTriggerFlag>(
            registry, [](entt::registry& registry, entt::entity entity) -> bool {
                return registry.view<EnemyFlag>().empty();
            });

    return any_scene_triggered;
}

// This function can get called by patching the TriggerComp.
// In many (most?) cases it's easier to use checkSceneTriggers once per frame.
void SceneSystem::onSceneTrigger(entt::registry& registry, entt::entity entity) {
    auto& pair_c = registry.get<PairComp>(entity);
    emplaceScene(registry, pair_c.target_entity);

    // Reset n_triggers
    auto& trigger_c = registry.get<TriggerComp>(entity);
    trigger_c.n_triggers = 0;
}

void SceneSystem::emplaceScene(entt::registry& registry,
                               const entt::entity scene_entity) {
    auto& scene_c = registry.get<SceneComp>(scene_entity);
    auto scene_name = registry.get<EntityName>(scene_entity);

    std::cout << "Emplacing scene: " << scene_c.scene_fp << std::endl;

    // Loop through and emplace entities
    // We keep track of the names for this scene so we can efficiently resolve them
    // during emplacement.
    EntityMap scene_entity_map;
    for (auto [entity_name, entity_components] : scene_c.json_data.items()) {
        scene_entity_map[entity_name] =
            emplaceEntity(registry, entity_name, entity_components);
    }

    // Resolve names
    auto rview = registry.view<UnresolvedNameFlag>();
    for (auto entity : rview) {
        // Check for each component that could have an unresolved name
        auto* pair_c_ptr = registry.try_get<PairComp>(entity);
        if (pair_c_ptr != nullptr) {
            pair_c_ptr->target_entity = resolveEntityName(
                registry, scene_entity_map, pair_c_ptr->target_entity_name);
            pair_c_ptr->source_entity = resolveEntityName(
                registry, scene_entity_map, pair_c_ptr->source_entity_name);
        }
        auto* vb_c_ptr = registry.try_get<ValueBarComp>(entity);
        if (vb_c_ptr != nullptr) {
            entt::entity tracked_entity = resolveEntityName(
                registry, scene_entity_map, vb_c_ptr->tracked_entity_name);
            vb_c_ptr->tracked_value =
                &registry.get<DurabilityComp>(tracked_entity).durability_frac;
        }
        registry.remove<UnresolvedNameFlag>(entity);
    }

    // Increment n_emplaced
    scene_c.n_emplaced++;
}

entt::entity SceneSystem::emplaceEntity(entt::registry& registry,
                                        const EntityName entity_name,
                                        const json& entity_components) {
    // Create an entity and store its name and ID
    auto entity = registry.create();
    registry.emplace<EntityName>(entity, entity_name);

    for (const auto& [comp_key, comp_json] : entity_components.items()) {
        comp::emplaceComponent(registry, entity, comp_key, comp_json);
    }

    return entity;
}

entt::entity SceneSystem::resolveEntityName(entt::registry& registry,
                                            EntityMap entity_map,
                                            EntityName entity_name) {
    // Parse as needed
    entt::entity entity;
    if (entity_name.front() == '[' && entity_name.back() == ']') {
        std::string input_str = entity_name.substr(1, entity_name.size() - 2);
        entity = comp::getEntityFromStr(registry, input_str);
    } else {
        entity = entity_map[entity_name];
    }

    // Otherwise return simply the name for the map
    if (!registry.valid(entity)) {
        throw std::runtime_error("Entity with name '" + entity_name + "' not found");
    }

    return entity;
}

}  // namespace twirl