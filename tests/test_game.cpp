#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>

#include "config.h"
#include "component.h"
#include "game.h"

using namespace twirl;

TEST(GameTest, Initialize) {

    // Initialize the game, hidden in the constructor
    Game game("../../tests/test_data/test_scene.json");

    // Check that the registry is not empty
    auto rview = game.registry.view<EntityName>();
    ASSERT_FALSE(rview.empty());

    // Check that the player entity is present
    entt::entity player_entity = game.entity_map["player"];
    ASSERT_FALSE(player_entity == entt::null);

    // Check that the player entity has the correct components
    auto& pc = game.registry.get<PhysicsComp>(player_entity);
    EXPECT_FLOAT_EQ(pc.mass, 1.0f);
}