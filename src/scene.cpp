#include "scene.h"

#include <entt/entity/component.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "component.h"
using json = nlohmann::ordered_json;

namespace twirl {

void SceneSystem::loadJsonData(entt::registry& registry) {
    for (auto [entity, sc] : registry.view<SceneComp>().each()) {
        // Parse the file
        std::ifstream file(sc.data_fp);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + sc.data_fp);
            return;
        }
        sc.json_data = json::parse(file);
    }
}

void SceneSystem::onSceneTrigger(entt::registry& registry, entt::entity entity) {
    auto& stc = registry.get<SceneTriggerComp>(entity);
    auto& sc = registry.get<SceneComp>(stc.scene_entity);

    // Loop through and emplace entities
    for (const auto& [entity_name, entity_json] : sc.json_data.items()) {
        emplaceEntity(registry, entity_name, entity_json);
    }

    // Reset n_triggers
    stc.n_triggers = 0;
}

void SceneSystem::emplaceEntity(entt::registry& registry, const std::string entity_name,
                                const json& entity_json) {
    // Create an entity and store its name and ID
    auto entity = registry.create();
    registry.emplace<EntityName>(entity, entity_name);

    json components = entity_json["components"];

    // DEBUG
    if (entity_name == "bkgrd.099") {
        std::cout << "bkgrd.099\n";
    }

    for (const auto& [comp_key, comp] : components.items()) {
        if (comp_key == "PhysicsComp") {
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
            EntityName target_entity_name = comp.at("target_entity");
            comp_inst.target_entity = entt::to_entity(registry, target_entity_name);
            EntityName source_entity_name = comp.at("source_entity");
            comp_inst.source_entity = entt::to_entity(registry, source_entity_name);
            registry.emplace<PairComp>(entity, comp_inst);
        } else if (comp_key == "PairwiseForceComp") {
            auto comp_inst = comp.template get<PairwiseForceComp>();
            registry.emplace<PairwiseForceComp>(entity, comp_inst);
        } else if (comp_key == "CollisionComp") {
            registry.emplace<CollisionComp>(entity);
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