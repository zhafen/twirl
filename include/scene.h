#ifndef SCENE_H
#define SCENE_H

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace cc {

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void loadFromJson(const std::string& filePath);

    entt::registry& getRegistry() { return registry; }

private:
    entt::registry registry;

    void parseEntity(const nlohmann::json& entityJson);
};

} // namespace cc

#endif // SCENE_H