#ifndef SCENE_H
#define SCENE_H

#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

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
    void onSceneTrigger(entt::registry& registry, entt::entity entity);

    json json_data;
    entt::registry registry;
    std::unordered_map<std::string, entt::entity> name_to_entity_map;

   private:
    void emplaceEntity(entt::registry& registry, const std::string entity_name,
                       const json& entity_json);
};

}  // namespace twirl

#endif  // SCENE_H