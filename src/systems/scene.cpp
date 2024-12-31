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

/**
 * @brief Checks for scene and state triggers based on various conditions.
 *
 * This function checks for different types of triggers within the registry.
 * It evaluates triggers caused by e.g. timers running out, durability reaching zero,
 * and the absence of enemies. If any of these conditions are met, the corresponding
 * triggers are activated.
 * 
 * A trigger is its own entity. It always has a flag indicating what type of
 * trigger it is, and a PairComp indicating the entity to evaluate the trigger
 * condition for (aka the source) and the scene entity containing the data to emplace.
 * If the trigger entity also has a StateComp, then instead of emplacing the entire
 * scene, only the state indicated by the StateComp is emplaced, and that modifies
 * the source entity.
 * 
 * Note: It's possible that we don't want to modify the triggering entity, but instead
 * another entity. If we want to do that, we'll need to modify StateComp to include
 * a "modified_entity_name" field, and then modify the emplaceState function to
 * check for the value of that field.
 *
 * @param registry The registry containing the entities and components.
 * @return True if any trigger is activated, otherwise false.
 */
bool SceneSystem::checkTriggers(entt::registry& registry) {
    bool any_triggered = false;

    // Triggers caused by a timer running out
    any_triggered =
        any_triggered |
        checkTriggersForFlag<WatchTriggerFlag>(
            registry, [](entt::registry& registry, entt::entity entity) -> bool {
                return registry.get<WatchComp>(entity).end_reached;
            });

    // Triggers caused by durability reaching 0
    any_triggered =
        any_triggered |
        checkTriggersForFlag<DurabilityTriggerFlag>(
            registry, [](entt::registry& registry, entt::entity entity) -> bool {
                return registry.get<DurabilityComp>(entity).durability <= 0;
            });

    // Triggers caused by no enemies existing
    any_triggered =
        any_triggered |
        checkTriggersForFlag<EnemyAbsenceTriggerFlag>(
            registry, [](entt::registry& registry, entt::entity entity) -> bool {
                return registry.view<EnemyFlag>().empty();
            });

    return any_triggered;
}

// This function can get called by patching the TriggerComp.
// In many (most?) cases it's easier to use checkTriggers once per frame.
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

    // Emplace the state
    emplaceState(registry, entity, entity_components);

    return entity;
}

void SceneSystem::emplaceState(entt::registry& registry,
                                        entt::entity entity,
                                        const json& state_components) {
    for (const auto& [comp_key, comp_json] : state_components.items()) {
        comp::emplaceComponent(registry, entity, comp_key, comp_json);
    }
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