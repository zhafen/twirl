
#include "scene.h"

#include <nlohmann/json.hpp>

namespace twirl {

void Scene::loadFromJson(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    nlohmann::json jsonData;
    file >> jsonData;

    for (const auto& entityJson : jsonData) {
        parseEntity(entityJson);
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