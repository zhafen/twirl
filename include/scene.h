#ifndef SCENE_H
#define SCENE_H

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace twirl {

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void loadFromJson(const std::string& filePath);

    entt::registry registry;

private:

    void parseEntity(const nlohmann::json& entityJson);
};

} // namespace twirl

#endif // SCENE_H