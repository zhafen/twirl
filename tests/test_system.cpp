#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>

#include "config.h"
#include "component.h"
#include "game.h"

using namespace twirl;

TEST(EntitySystemTest, SpawnDeleteOrder) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& entity_system = game.getEntitySystem();

    // Try spawning, deleting, and ordering entities
    entity_system.deleteEntities(registry);
    entity_system.spawnEntities(registry);
    entity_system.orderEntities(registry);
}
