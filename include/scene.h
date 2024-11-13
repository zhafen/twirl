#ifndef SCENE_H
#define SCENE_H

#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace twirl {

class Scene {
   public:
    Scene() = default;
    ~Scene() = default;

    void loadFromJson(const std::string& filePath);

    entt::registry registry;

   private:
    void parseEntity(std::string entity_name, const nlohmann::json& entity_json,
                     std::unordered_map<std::string, entt::entity>& entity_map);
};

}  // namespace twirl

#endif  // SCENE_H