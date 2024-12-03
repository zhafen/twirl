#include "systems/entity.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <iostream>

#include "components/components.h"
#include "config.h"
#include "shape.h"

namespace twirl {

EntityMap& EntitySystem::getEntityMap(entt::registry& registry) {
    // Clear the map to start
    entity_map.clear();

    auto rview = registry.view<EntityName>();
    for (auto [entity, name] : rview.each()) {
        entity_map.emplace(name, entity);
    }

    return entity_map;
}
// TODO: Reimplement or fully deprecate.
// entt::entity EntitySystem::resolveEntityName(entt::registry& registry, EntityName
// name,
//                                entt::entity entity) {
//     // If there's already a valid entity, return it
//     if (registry.valid(entity)) {
//         return entity;
//     }
//
//     // If the the name is not empty, try to resolve it
//     if (!name.empty()) {
//         // Try to get the entity name from the entity map
//         if (entity_map.find(name) != entity_map.end()) {
//             return entity_map.at(name);
//         }
//     }
//     return entt::null;
// }
//
// void EntitySystem::resolveEntityNames(entt::registry& registry) {
//     resolveSceneTriggerNames(registry);
//     resolvePairNames(registry);
// }
//
// void EntitySystem::resolveSceneTriggerNames(entt::registry& registry) {
//     auto rview = registry.view<SceneTriggerComp>();
//     for (auto [entity, scenetrigger_c] : rview.each()) {
//         scenetrigger_c.scene_entity = resolveEntityName(registry,
//         scenetrigger_c.scene_name,
//                                                         scenetrigger_c.scene_entity);
//     }
// }
//
// /**
//  * OPTIMIZE: This function may not be able to loop over every paircomp if we use
//  * listeners, and should probably not call getEntityMap every time.
//  */
// void EntitySystem::resolvePairNames(entt::registry& registry) {
//     auto rview = registry.view<PairComp>();
//     for (auto [pair_entity, pair_c] : rview.each()) {
//         pair_c.target_entity = resolveEntityName(registry, pair_c.target_entity_name,
//                                                  pair_c.target_entity);
//         pair_c.source_entity = resolveEntityName(registry, pair_c.source_entity_name,
//                                                  pair_c.source_entity);
//         // If either entity is not resolved after trying to resolve the names,
//         // destroy the pair entity
//         if ((pair_c.target_entity == entt::null) ||
//             (pair_c.source_entity == entt::null)) {
//             registry.destroy(pair_entity);
//         }
//     }
// }

void EntitySystem::spawnEntities(entt::registry& registry) {
    auto rview = registry.view<StopWatchComp, SceneTriggerComp>();
    for (auto [entity, stopwatch_c, scenetrigger_c] : rview.each()) {
        // Check if the end time was reached
        if (!stopwatch_c.end_reached) {
            continue;
        } else {
            // Reset the stopwatch
            stopwatch_c.current_time = 0.0f;
            stopwatch_c.end_reached = false;
        }

        // If we got this far then we activate the scene trigger
        registry.patch<SceneTriggerComp>(entity, [](auto& stc) { stc.n_triggers++; });

        // Spawning means we need to reorder
        needs_ordering = true;
    }
}

void EntitySystem::deleteEntities(entt::registry& registry) {
    auto rview = registry.view<DeleteComp>();

    for (auto entity : rview) {
        registry.destroy(entity);
        needs_ordering = true;
    }
}

void EntitySystem::orderEntities(entt::registry& registry) {
    if (!needs_ordering) {
        return;
    }

    registry.sort<RenderComp>(
        [](const auto lhs, const auto rhs) { return lhs.zorder < rhs.zorder; });

    // Mark that the entities are ordered
    needs_ordering = false;
}

/**
 * @brief Syncs entity values
 *
 * Currently I have to hardcode each combination of synced values.
 * Generalizing this requires reflection, which is perhaps easiest solved
 * by writing Python to generate the functions.
 *
 * @param registry The registry containing all entities and components.
 */
void EntitySystem::syncEntities(entt::registry& registry) {
    // Sync entity positions
    auto rview = registry.view<PairComp, SyncPositionComp>();
    for (auto [rel_entity, pair_c, syncpos_c] : rview.each()) {
        PhysicsComp& target_phys_c = registry.get<PhysicsComp>(pair_c.target_entity);
        PhysicsComp source_phys_c = registry.get<PhysicsComp>(pair_c.source_entity);
        target_phys_c.pos = source_phys_c.pos;

        // Destroy the relationship if it's a one-time sync
        if (syncpos_c.once_only) {
            registry.remove<SyncPositionComp>(rel_entity);
        }
    }
}

}  // namespace twirl