#include "scene.h"
#include "component.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
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
    // Create an entity and store its name
    auto entity = registry.create();
    registry.emplace<MetadataComp>(entity, entity_name);

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
        } else {
            throw std::runtime_error("Unknown component key: " + comp_key);
        }
    }
}

} // namespace twirl