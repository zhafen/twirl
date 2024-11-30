#ifndef TEST_GENERATED_COMPONENTS_H
#define TEST_GENERATED_COMPONENTS_H

#include <string>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct EnemyComp {};

struct SceneComp {
    std::string scene_fp;
    bool emplace_after_loading = false;
    json json_data;
    size_t n_emplaced = 0;
    bool verbose_names = true;
};
inline void from_json(const json& j, SceneComp& scenecomp) {
    scenecomp.scene_fp = j.at("scene_fp").get<std::string>();
    scenecomp.emplace_after_loading = j.value("emplace_after_loading", false);
    scenecomp.verbose_names = j.value("verbose_names", true);
}

struct PhysicsComp {
    float mass = 1.0f;
    sf::Vector2f pos = sf::Vector2f(0.0f, 0.0f) * cfg.L;
    sf::Vector2f vel = sf::Vector2f(0.0f, 0.0f) * cfg.V;
    sf::Vector2f force = sf::Vector2f(0.0f, 0.0f) * cfg.A;
    bool collided = false;
};
inline void from_json(const json& j, PhysicsComp& physicscomp) {
    physicscomp.mass = j.value("mass", 1.0f);
    physicscomp.pos = j.value("pos", sf::Vector2f(0.0f, 0.0f)) * cfg.L;
    physicscomp.vel = j.value("vel", sf::Vector2f(0.0f, 0.0f)) * cfg.V;
}

}  // namespace twirl

#endif  // TEST_GENERATED_COMPONENTS_H