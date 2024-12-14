#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>

#include "config.h"
#include "components/components.h"
#include "game.h"

using namespace twirl;

float MAX_TIME = 0.5f;

TEST(ScenarioTest, TestScene) {
    // Initialize the game, hidden in the constructor
    Game game("../../tests/test_data/test_scenes/main_test_scene.json");
    game.max_time = MAX_TIME;
    game.run();
}

TEST(ScenarioTest, TestMenuScene) {

    // Initialize the game, hidden in the constructor
    Game game("../../tests/test_data/test_scenes/test_menu.json");
    game.max_time = MAX_TIME;
    game.run();
}