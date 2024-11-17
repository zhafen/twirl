#include "scene.h"

#include <entt/entity/helper.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "component.h"

using json = nlohmann::ordered_json;

namespace twirl {

void SceneSystem::loadJsonData(entt::registry& registry) {
    for (auto [entity, sc] : registry.view<SceneComp>().each()) {
        // Parse the file
        std::ifstream file(sc.scene_fp);
        if (!file.is_open()) {
            throw std::runtime_error(
                "Failed to open file: " + sc.scene_fp +
                "\nCurrent directory: " + std::filesystem::current_path().string());
            return;
        }
        sc.json_data = json::parse(file);

        if (sc.emplace_after_loading) {
            emplaceScene(registry, entity);
        }
    }
}

void SceneSystem::onSceneTrigger(entt::registry& registry, entt::entity entity) {
    auto& stc = registry.get<SceneTriggerComp>(entity);
    emplaceScene(registry, stc.scene_entity);

    // Reset n_triggers
    stc.n_triggers = 0;
}

void SceneSystem::emplaceScene(entt::registry& registry,
                               const entt::entity scene_entity) {
    auto& sc = registry.get<SceneComp>(scene_entity);

    // Loop through and emplace entities
    EntityMap scene_entity_map;
    for (const auto& [entity_name, entity_json] : sc.json_data.items()) {
        emplaceEntity(registry, scene_entity_map, entity_name, entity_json);
    }
}

void SceneSystem::emplaceEntity(entt::registry& registry, EntityMap& scene_entity_map, const std::string entity_name,
                                const json& entity_json) {
    // Create an entity and store its name and ID
    auto entity = registry.create();
    registry.emplace<EntityName>(entity, entity_name);
    // We also keep track of the inverse so we can parse PairComp
    scene_entity_map[entity_name] = entity;

    json components = entity_json["components"];

    for (const auto& [comp_key, comp] : components.items()) {
        if (comp_key == "SceneComp") {
            auto comp_inst = comp.template get<SceneComp>();
            registry.emplace<SceneComp>(entity, comp_inst);
        } else if (comp_key == "PhysicsComp") {
            auto comp_inst = comp.template get<PhysicsComp>();
            registry.emplace<PhysicsComp>(entity, comp_inst);
        } else if (comp_key == "DragForceComp") {
            auto comp_inst = comp.template get<DragForceComp>();
            registry.emplace<DragForceComp>(entity, comp_inst);
        } else if (comp_key == "DurabilityComp") {
            auto comp_inst = comp.template get<DurabilityComp>();
            registry.emplace<DurabilityComp>(entity, comp_inst);
        } else if (comp_key == "MouseButtonReleasedComp") {
            registry.emplace<MouseButtonReleasedComp>(entity);
        } else if (comp_key == "RenderComp") {
            auto comp_inst = comp.template get<RenderComp>();
            registry.emplace<RenderComp>(entity, comp_inst);
        } else if (comp_key == "EnemyComp") {
            registry.emplace<EnemyComp>(entity);
        } else if (comp_key == "DeleteComp") {
            registry.emplace<DeleteComp>(entity);
        } else if (comp_key == "PairComp") {
            auto comp_inst = PairComp();
            comp_inst.target_entity = scene_entity_map.at(comp.at("target_entity"));
            comp_inst.source_entity = scene_entity_map.at(comp.at("source_entity"));
            registry.emplace<PairComp>(entity, comp_inst);
        } else if (comp_key == "PairwiseForceComp") {
            auto comp_inst = comp.template get<PairwiseForceComp>();
            registry.emplace<PairwiseForceComp>(entity, comp_inst);
        } else if (comp_key == "CollisionComp") {
            registry.emplace<CollisionComp>(entity);
        } else if (comp_key == "ViewComp") {
            registry.emplace<ViewComp>(entity);
        } else if (comp_key == "UIComp") {
            auto comp_inst = comp.template get<UIComp>();
            registry.emplace<UIComp>(entity, comp_inst);
        } else if (comp_key == "StopWatchComp") {
            auto comp_inst = comp.template get<StopWatchComp>();
            registry.emplace<StopWatchComp>(entity, comp_inst);
        } else {
            throw std::runtime_error("Unknown component key: " + comp_key);
        }
    }
}

}  // namespace twirl