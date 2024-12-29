#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>

#include "components/components.h"
#include "config.h"
#include "game.h"

using namespace twirl;

float MAX_TIME = 1.1f;

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
    EXPECT_NO_THROW({
        game.run();
        standardValidityChecks(game.getRegistry());
    });
}

TEST(ScenarioTest, TestScene) {
    Game game("../../tests/test_data/test_scenes/main_test_scene.json");
    game.max_time = MAX_TIME;
    EXPECT_NO_THROW({
        game.run();
        standardValidityChecks(game.getRegistry());
    });
}

TEST(ScenarioTest, TestCollision) {
    Game game("../../tests/test_data/test_scenes/test_collision.json");
    game.max_time = MAX_TIME;
    entt::registry& registry = game.getRegistry();
    entt::entity entity1 =
        comp::getEntityFromStr(registry, "PhysicsComp|name:entity1");
    entt::entity entity2 =
        comp::getEntityFromStr(registry, "PhysicsComp|name:entity2");
    EXPECT_NO_THROW({
        game.run();
        standardValidityChecks(registry);
    });
    ASSERT_TRUE(registry.valid(entity1));
    ASSERT_FALSE(registry.valid(entity2));
}

TEST(ScenarioTest, TestMenuScene) {
    Game game("../../tests/test_data/test_scenes/test_menu.json");
    game.max_time = MAX_TIME;
    EXPECT_NO_THROW({
        game.run();
    });
}