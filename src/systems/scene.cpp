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
    for (auto [entity_name_base, entity_json] : scene_c.json_data.items()) {
        EntityName entity_name;
        if (scene_c.verbose_names) {
            entity_name = scene_name + "." + std::to_string(scene_c.n_emplaced) + "." + entity_name_base;
        } else {
            entity_name = entity_name_base;
        }
        emplaceEntity(registry, entity_name, entity_json);
    }

    // Increment n_emplaced
    scene_c.n_emplaced++;
}

void SceneSystem::emplaceEntity(entt::registry& registry,
                                const EntityName entity_name,
                                const json& entity_json) {
    // Create an entity and store its name and ID
    auto entity = registry.create();
    registry.emplace<EntityName>(entity, entity_name);

    // TODO:Add per-component tests so we can track down when emplacing
    // just one of the components is failing
    json components = entity_json["components"];
    for (const auto& [comp_key, comp_json] : components.items()) {
        comp::emplaceComponent(registry, entity, comp_key, comp_json);
    }
}

}  // namespace twirl