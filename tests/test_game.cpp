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
    auto rview = game.getRegistry().view<EntityName>();
    ASSERT_FALSE(rview.empty());

    // Check that the player entity is present
    entt::entity player_entity = game.getEntityMap()["player"];
    ASSERT_FALSE(player_entity == entt::null);

    // Check that the player entity has the correct components
    auto& pc = game.getRegistry().get<PhysicsComp>(player_entity);
    EXPECT_FLOAT_EQ(pc.mass, 1.0f);
}

TEST(GameTest, HandleEvents) {
    // Initialize the game
    Game game("../../tests/test_data/test_scene.json");

    // Get window
    sf::RenderWindow& window = game.getWindow();
    ASSERT_TRUE(window.isOpen());

    // Run the handle events function once
    game.handleEvents();

    // Close the window
    window.close();
}

TEST(GameTest, Update) {
    // Initialize the game
    Game game("../../tests/test_data/test_scene.json");

    // Get window
    sf::RenderWindow& window = game.getWindow();
    ASSERT_TRUE(window.isOpen());

    // Run the handle events function once
    game.update();

    // Close the window
    window.close();
}