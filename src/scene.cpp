
#include "scene.h"

#include <nlohmann/json.hpp>

namespace twirl {

void Scene::loadFromJson(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    nlohmann::json json_data;
    file >> json_data;

    for (const auto& entity_json : json_data) {
        parseEntity(entity_json);
    }
}

void Scene::parseEntity(const nlohmann::json& entityJson) {
    auto entity = registry.create();
    // Assuming entityJson contains components data
    // Add components to the entity based on the JSON data
    // Example:
    // if (entityJson.contains("transform")) {
    //     registry.emplace<TransformComponent>(entity, entityJson["transform"]);
    // }

}

} // namespace twirl