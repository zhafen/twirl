#ifndef SCENE_H
#define SCENE_H

#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace twirl {

class SceneSystem {
   public:
    SceneSystem() = default;
    ~SceneSystem() = default;

    void loadFromJson(const std::string& filePath);

    entt::registry registry;
    std::unordered_map<std::string, entt::entity> name_to_entity_map;

   private:
    void parseEntity(std::string entity_name, const nlohmann::ordered_json& entity_json);
};

}  // namespace twirl

#endif  // SCENE_H