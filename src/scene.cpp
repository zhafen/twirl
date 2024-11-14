#include "scene.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "component.h"
using json = nlohmann::json;

namespace twirl {

void Scene::loadFromJson(const std::string& file_path) {
    // Parse the file
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path);
        return;
    }
    json json_data = json::parse(file);

    // Loop through and parse entities
    for (const auto& [entity_name, entity_json] : json_data.items()) {
        parseEntity(entity_name, entity_json);
    }
}

void Scene::parseEntity(const std::string entity_name, const json& entity_json) {
    // Create an entity and store its name and ID
    auto entity = registry.create();
    registry.emplace<MetadataComp>(entity, entity_name);
    name_to_entity_map[entity_name] = entity;

    json components = entity_json["components"];

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
        } else if (comp_key == "SpawnComp") {
            registry.emplace<SpawnComp>(entity);
        } else if (comp_key == "PairComp") {
            auto comp_inst = PairComp();
            comp_inst.target_entity = name_to_entity_map.at(comp.at("target_entity"));
            comp_inst.source_entity = name_to_entity_map.at(comp.at("source_entity"));
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