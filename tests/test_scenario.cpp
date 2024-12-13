#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>

#include "config.h"
#include "components/components.h"
#include "game.h"

using namespace twirl;

TEST(ScenarioTest, TestScene) {

    // Initialize the game, hidden in the constructor
    Game game("../../tests/test_data/test_scenes/main_test_scene.json");
    game.run();
}