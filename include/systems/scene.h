#ifndef SCENE_H
#define SCENE_H

#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "components/components.h"

using json = nlohmann::ordered_json;

namespace twirl {

/**
 *
 * Scene system requirements (+ possible solutions indented):
 * - Load entities from a json file
 * - Store the entities in memory
 *   - Store the entities in one registry per scene component
 *   - Store the entities in in one registry for all scenes
 *   - Store the entities in the main registry
 *   - Store the entities in an entity pool
 * - Add to the main registry 1+ copies of the entities
 *   - Copy from memory
 *   - Spawn directly in the main registry (if I'm trying to save expenses by
 *     preloading, it doesn't make sense to then have the expense of copying them,
 *     since I don't think that's any cheaper than just creating them)
 * - Delete all entities associated with the scene
 *    - Maybe this isn't essential. I can use other tag components instead. If I wanted
 *      to delete entities associated with a given scene I would have to keep track of
 *      an arbitrary number of unique scenes, and I couldn't easily use a unique
 *      component to tag them.
 *
 * With the above in mind, I think the best solution is to store the raw data as a
 * loaded json, and then use that to create the entities in the main registry.
 *
 */
class SceneSystem {
   public:
    SceneSystem() = default;
    ~SceneSystem() = default;

    void loadJsonData(entt::registry& registry);
    bool checkSceneTriggers(entt::registry& registry);
    void onSceneTrigger(entt::registry& registry, entt::entity entity);
    void emplaceScene(entt::registry& registry, entt::entity scene_entity);
    entt::entity emplaceEntity(entt::registry& registry, const EntityName entity_name,
                               const json& entity_components);
    entt::entity resolveEntityName(entt::registry& registry, EntityMap entity_map,
                                   EntityName entity_name);

    json json_data;

   private:
    // This function checks check_func for all entities with a given flag
    // to see if those triggers should be triggered
    template <typename TriggerFlag, typename Func>
    bool checkSceneTriggersForFlag(entt::registry& registry, Func check_func) {
        // Set up loop
        bool any_scene_triggered = false;
        auto rview = registry.view<TriggerFlag, PairComp>();
        for (auto [entity, pair_c] : rview.each()) {
            // Check if the scene was triggered
            bool triggered = check_func(registry, pair_c.source_entity);
            if (!triggered) {
                continue;
            }

            // Emplace the scene
            emplaceScene(registry, pair_c.target_entity);
            any_scene_triggered = true;
        }

        return any_scene_triggered;
    }
};

}  // namespace twirl

#endif  // SCENE_H