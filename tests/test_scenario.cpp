#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>

#include "config.h"
#include "components/components.h"
#include "game.h"

using namespace twirl;

float MAX_TIME = 1.5f;

void standardValidityChecks(entt::registry& registry) {
    comp::debugEntities(registry, "Debugging inside standardValidityChecks");
    auto rview = registry.view<PhysicsComp>();
    int n_nan = 0;
    for (auto [entity, phys_c] : rview.each()) {
        if (std::isnan(phys_c.pos.x) || std::isnan(phys_c.pos.y)) {

            auto entity_name_ptr = registry.try_get<EntityName>(entity);
            if (entity_name_ptr != nullptr) {
                std::cerr << "Warning: Entity " << *entity_name_ptr
                          << " has NaN position.\n";
            } else {    
            std::cerr << "Warning: Entity " << static_cast<int>(entity)
                        << " has NaN position.\n";
            }
            n_nan++;
        }
    }
    ASSERT_EQ(n_nan, 0);
}

TEST(ScenarioTest, TestMain) {
    Game game("../../scenes/main_scene.json");
    game.max_time = MAX_TIME;
    game.run();
    standardValidityChecks(game.getRegistry());
}

TEST(ScenarioTest, TestScene) {
    Game game("../../tests/test_data/test_scenes/main_test_scene.json");
    game.max_time = MAX_TIME;
    game.run();
    standardValidityChecks(game.getRegistry());
}

TEST(ScenarioTest, TestCollision) {
    Game game("../../tests/test_data/test_scenes/test_collision.json");
    // game.max_time = MAX_TIME;
    game.max_time = 500.f;
    game.run();
    standardValidityChecks(game.getRegistry());
}

TEST(ScenarioTest, TestMenuScene) {
    Game game("../../tests/test_data/test_scenes/test_menu.json");
    game.max_time = MAX_TIME;
    game.run();
}