#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>

#include "config.h"
#include "component.h"
#include "game.h"

using namespace twirl;

TEST(SystemEntityTest, SpawnDeleteOrder) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& entity_system = game.getEntitySystem();

    // Try spawning, deleting, and ordering entities
    entity_system.deleteEntities(registry);
    entity_system.spawnEntities(registry);
    entity_system.orderEntities(registry);
}

TEST(SystemEntityTest, ResolveEntityNames) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& entity_system = game.getEntitySystem();

    // Resolve entity names
    entity_system.resolveEntityNames(registry);

    // Check that the entities match up
    auto entity_map = game.getEntityMap();
    auto rview = registry.view<EntityName, PairComp>();
    for (auto [pair_entity, pair_entity_name, pc] : rview.each()) {
        if (pair_entity_name == "player-beacon force") {
            ASSERT_EQ(entity_map["player"], pc.target_entity);
            ASSERT_EQ(entity_map["beacon"], pc.source_entity);
        }
    }
}

TEST(SystemPhysicsTest, CalculateForces) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Calculate forces
    physics_system.calculateForces(registry);
}

TEST(SystemPhysicsTest, CalculatePairwiseForces) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();
    // Need to resolve entity names before calculating pairwise forces
    game.getEntitySystem().resolveEntityNames(registry);

    // Calculate forces
    physics_system.calculatePairwiseForces(registry);
}

TEST(SystemPhysicsTest, Update) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Update state
    physics_system.update(registry);
}

TEST(SystemPhysicsTest, UpdateStopWatches) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Update stop watches
    physics_system.updateStopWatches(registry);
}

TEST(SystemPhysicsTest, ResolveCollisions) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Resolve collisions
    physics_system.resolveCollisions(registry);
}

TEST(SystemPhysicsTest, UpdateDurability) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Update durability
    physics_system.updateDurability(registry);
}

TEST(SystemRenderTest, Render) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& render_system = game.getRenderSystem();
    auto& window = game.getWindow();

    // Render
    render_system.render(registry, window);
}

TEST(SystemRenderTest, RenderUI) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& render_system = game.getRenderSystem();
    auto& window = game.getWindow();

    // Render
    render_system.render(registry, window);
    render_system.renderUI(registry, window);
    render_system.setView(registry, window, game.getView());
}

TEST(SystemRenderTest, SetView) {

    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& render_system = game.getRenderSystem();
    auto& window = game.getWindow();

    // Render
    render_system.setView(registry, window, game.getView());
}