#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>

#include "component.h"
#include "config.h"
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

TEST(SystemEntityTest, StopWatchSpawn) {

    entt::registry registry;
    EntitySystem entity_system;

    // Ready a scene to spawn
    entt::entity scene_entity = registry.create();
    registry.emplace<EntityName>(scene_entity, "test_scene");
    auto& sc = registry.emplace<SceneComp>(scene_entity);
    sc.json_data = R"(
    {
        "spawned_entity": {
            "components": {
                "PhysicsComp": {},
                "RenderComp": {},
            }
        }
    }
    )"_json;

    // Spawning entity
    entt::entity spawner_entity = registry.create();
    registry.emplace<SceneTriggerComp>(spawner_entity, "test_scene");
    registry.emplace<PhysicsComp>(spawner_entity, 1.0f, sf::Vector2f(10.f, 10.f));
    auto& swc = registry.emplace<StopWatchComp>(spawner_entity);
    // We test stopwatch updates separately, so we set the trigger as ready to go
    swc.end_reached = true;

    // Trigger
    entity_system.spawnEntities(registry);

    // Check that the entity was created properly
    EntityMap entity_map = entity_system.getEntityMap(registry);
    entt::entity spawned_entity = entity_map["spawned_entity"];
    ASSERT_TRUE(registry.valid(spawned_entity));
    auto& pc = registry.get<PhysicsComp>(spawned_entity);
    ASSERT_FLOAT_EQ(pc.mass, 1.0f);

    // For this spawn the location of the spawned entity should be that of the spawner
    auto& pc_spawner = registry.get<PhysicsComp>(spawner_entity);
    ASSERT_FLOAT_EQ(pc.pos.x, pc_spawner.pos.x);
    ASSERT_FLOAT_EQ(pc.pos.y, pc_spawner.pos.y);
}

TEST(SystemEntityTest, ResolveEntityNames) {
    // Initialize the game in its test state
    Game game("../../tests/test_data/test_scene.json");
    auto& registry = game.getRegistry();
    auto& entity_system = game.getEntitySystem();

    // Resolve entity names
    entity_system.resolveEntityPairs(registry);

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

TEST(SystemEntityPhysicsTest, ResolveEntityPairs) {
    // Set up test objects
    entt::registry registry;
    EntitySystem entity_system;
    PhysicsSystem physics_system;
    // Entities themselves
    entt::entity entity1 = registry.create();
    registry.emplace<EntityName>(entity1, "entity1");
    registry.emplace<PhysicsComp>(entity1);
    entt::entity entity2 = registry.create();
    registry.emplace<EntityName>(entity2, "entity2");
    registry.emplace<PhysicsComp>(entity2);
    entt::entity entity3 = registry.create();
    registry.emplace<EntityName>(entity3, "entity3");
    registry.emplace<PhysicsComp>(entity3);
    // Entity-entity relationships
    entt::entity rel_12 = registry.create();
    PairComp pc_12;
    pc_12.target_entity_name = "entity1";
    pc_12.source_entity_name = "entity2";
    registry.emplace<PairComp>(rel_12, pc_12);
    registry.emplace<PairwiseForceComp>(rel_12);
    entt::entity rel_23 = registry.create();
    PairComp pc_23;
    pc_23.target_entity_name = "entity2";
    pc_23.source_entity_name = "entity3";
    registry.emplace<PairComp>(rel_23, pc_23);
    registry.emplace<PairwiseForceComp>(rel_23);
    entt::entity rel_31 = registry.create();
    PairComp pc_31;
    pc_31.target_entity_name = "entity3";
    pc_31.source_entity_name = "entity1";
    registry.emplace<PairComp>(rel_31, pc_31);
    registry.emplace<PairwiseForceComp>(rel_31);

    // Resolve names then forces
    entity_system.resolveEntityPairs(registry);
    physics_system.calculatePairwiseForces(registry);
    ASSERT_TRUE(registry.valid(rel_12));
    ASSERT_TRUE(registry.valid(rel_23));
    ASSERT_TRUE(registry.valid(rel_31));
    PairComp pc_12_now = registry.get<PairComp>(rel_12);
    PairComp pc_23_now = registry.get<PairComp>(rel_23);
    PairComp pc_31_now = registry.get<PairComp>(rel_31);
    ASSERT_EQ(pc_12_now.target_entity, entity1);
    ASSERT_EQ(pc_12_now.source_entity, entity2);
    ASSERT_EQ(pc_23_now.target_entity, entity2);
    ASSERT_EQ(pc_23_now.source_entity, entity3);
    ASSERT_EQ(pc_31_now.target_entity, entity3);
    ASSERT_EQ(pc_31_now.source_entity, entity1);

    // Delete an entity and try again
    registry.destroy(entity1);
    entity_system.resolveEntityPairs(registry);
    physics_system.calculatePairwiseForces(registry);
    ASSERT_TRUE(registry.valid(rel_23));
    PairComp pc_23_final = registry.get<PairComp>(rel_23);
    ASSERT_EQ(pc_23_final.target_entity, entity2);
    ASSERT_EQ(pc_23_final.source_entity, entity3);
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
    game.getEntitySystem().resolveEntityPairs(registry);

    // Calculate forces
    physics_system.calculatePairwiseForces(registry);
}

TEST(SystemPhysicsTest, CalculatePairwiseForcesPostDelete) {
    // Set up test objects
    entt::registry registry;
    PhysicsSystem physics_system;
    EntitySystem entity_system;
    // Entities themselves
    entt::entity entity1 = registry.create();
    registry.emplace<PhysicsComp>(entity1);
    entt::entity entity2 = registry.create();
    registry.emplace<PhysicsComp>(entity2);
    entt::entity entity3 = registry.create();
    registry.emplace<PhysicsComp>(entity3);
    // Entity-entity relationships
    entt::entity rel_12 = registry.create();
    registry.emplace<PairComp>(rel_12, entity1, entity2);
    registry.emplace<PairwiseForceComp>(rel_12);
    entt::entity rel_23 = registry.create();
    registry.emplace<PairComp>(rel_23, entity2, entity3);
    registry.emplace<PairwiseForceComp>(rel_23);
    entt::entity rel_31 = registry.create();
    registry.emplace<PairComp>(rel_31, entity3, entity1);
    registry.emplace<PairwiseForceComp>(rel_31);

    // Calculate forces
    entity_system.resolveEntityPairs(registry);
    physics_system.calculatePairwiseForces(registry);

    // Delete an entity and try again
    registry.destroy(entity1);
    entity_system.resolveEntityPairs(registry);
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