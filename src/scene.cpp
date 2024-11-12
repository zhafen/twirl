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
        std::cerr << "Failed to open file: " << file_path << std::endl;
        std::string cwd = std::filesystem::current_path();
        std::string path = "../../tests/test_";
        for (const auto & entry : std::filesystem::directory_iterator(path)) {
            std::cout << entry.path() << std::endl;
        }
        std::cerr << "Current path: " << cwd << std::endl;
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
        if (comp_key == "DragForceComp") {
            auto comp_inst = comp.template get<DragForceComp>();
            registry.emplace<DragForceComp>(entity, comp_inst);
        }
    }

    // Assuming entityJson contains components data
    // Add components to the entity based on the JSON data
    // Example:
    // if (entityJson.contains("transform")) {
    //     registry.emplace<TransformComponent>(entity, entityJson["transform"]);
    // }

}

} // namespace twirl