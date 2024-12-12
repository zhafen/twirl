#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>

#include "components/components.h"
#include "config.h"
#include "game.h"

using namespace twirl;

TEST(SystemEntityTest, GetEntityMap) {
    entt::registry registry;
    EntitySystem entity_system;

    // Create entities with names
    entt::entity entity1 = registry.create();
    registry.emplace<EntityName>(entity1, "entity1");
    entt::entity entity2 = registry.create();
    registry.emplace<EntityName>(entity2, "entity2");
    entt::entity entity3 = registry.create();
    registry.emplace<EntityName>(entity3, "entity3");

    // Get the entity map
    EntityMap entity_map = entity_system.getEntityMap(registry);

    // Check that the entity map contains the correct entities
    EXPECT_EQ(entity_map["entity1"], entity1);
    EXPECT_EQ(entity_map["entity2"], entity2);
    EXPECT_EQ(entity_map["entity3"], entity3);
    EXPECT_THROW(entity_map.at("entity4"), std::out_of_range);
    EXPECT_TRUE(entity_map.find("entity4") == entity_map.end());
}

TEST(SystemEntityTest, GetEntityMapDeleted) {
    entt::registry registry;
    EntitySystem entity_system;

    // Create entities with names
    entt::entity entity1 = registry.create();
    registry.emplace<EntityName>(entity1, "entity1");
    entt::entity entity2 = registry.create();
    registry.emplace<EntityName>(entity2, "entity2");
    entt::entity entity3 = registry.create();
    registry.emplace<EntityName>(entity3, "entity3");

    // Get the entity map
    EntityMap entity_map = entity_system.getEntityMap(registry);

    // Check that the entity map contains the correct entities
    EXPECT_EQ(entity_map["entity1"], entity1);
    EXPECT_EQ(entity_map["entity2"], entity2);
    EXPECT_EQ(entity_map["entity3"], entity3);
    EXPECT_THROW(entity_map.at("entity4"), std::out_of_range);
    EXPECT_TRUE(entity_map.find("entity4") == entity_map.end());

    // Delete an entity and get the entity map again
    registry.destroy(entity2);
    entity_map = entity_system.getEntityMap(registry);
    EXPECT_EQ(entity_map["entity1"], entity1);
    EXPECT_THROW(entity_map.at("entity2"), std::out_of_range);
    EXPECT_TRUE(entity_map.find("entity2") == entity_map.end());
    EXPECT_EQ(entity_map["entity3"], entity3);
    EXPECT_THROW(entity_map.at("entity4"), std::out_of_range);
    EXPECT_TRUE(entity_map.find("entity4") == entity_map.end());

}

// TODO: Implement or fully deprecate this.
// TEST(SystemEntityTest, ResolveEntityName) {
//     entt::registry registry;
//     EntitySystem entity_system;
// 
//     // Create an entity with a name
//     entt::entity entity = registry.create();
//     registry.emplace<EntityName>(entity, "entity");
// 
//     // Get the mapping
//     entity_system.getEntityMap(registry);
// 
//     // Resolve the entity name
//     entt::entity resolved_entity =
//         entity_system.resolveEntityName(registry, "entity", entt::null);
// 
//     // Check that the entity was resolved
//     EXPECT_TRUE(entity == resolved_entity);
// }
// 
// TEST(SystemEntityTest, ResolveEntityNameInvalid) {
//     entt::registry registry;
//     EntitySystem entity_system;
// 
//     // Get the mapping
//     entity_system.getEntityMap(registry);
// 
//     // Resolve the entity name
//     entt::entity resolved_entity =
//         entity_system.resolveEntityName(registry, "entity", entt::null);
// 
//     // Check that the entity wasn't resolved
//     EXPECT_TRUE(resolved_entity == entt::null);
//     EXPECT_FALSE(registry.valid(resolved_entity));
// }
// 
// TEST(SystemEntityTest, ResolveEntityNameDeleted) {
//     entt::registry registry;
//     EntitySystem entity_system;
// 
//     // Create an entity with a name
//     entt::entity entity = registry.create();
//     registry.emplace<EntityName>(entity, "entity");
// 
//     // Resolve the entity name
//     entity_system.getEntityMap(registry);
//     entt::entity resolved_entity =
//         entity_system.resolveEntityName(registry, "entity", entt::null);
// 
//     // Check that the entity was resolved
//     EXPECT_FALSE(resolved_entity == entt::null);
//     EXPECT_TRUE(registry.valid(resolved_entity));
// 
//     // Delete the entity and try again
//     registry.destroy(entity);
//     entity_system.getEntityMap(registry);
//     entt::entity resolved_entity_final =
//         entity_system.resolveEntityName(registry, "entity", entt::null);
// 
//     // Check that the entity wasn't resolved
//     EXPECT_TRUE(resolved_entity_final == entt::null);
//     EXPECT_FALSE(registry.valid(resolved_entity_final));
// }

TEST(SystemEntityTest, SpawnDeleteOrder) {
    // Initialize the game in its test state
    Game game("../../scenes/main_scene.json");
    auto& registry = game.getRegistry();
    auto& entity_system = game.getEntitySystem();
    auto& scene_system = game.getSceneSystem();

    // Try spawning, deleting, and ordering entities
    entity_system.deleteEntities(registry);
    scene_system.checkSceneTriggers(registry);
    entity_system.orderEntities(registry);
}

TEST(SystemEntityTest, WatchSpawn) {
    entt::registry registry;
    EntitySystem entity_system;
    SceneSystem scene_system;
    registry.on_update<TriggerComp>().connect<&SceneSystem::onSceneTrigger>(
        scene_system);

    // Ready a scene to spawn
    entt::entity scene_entity = registry.create();
    registry.emplace<EntityName>(scene_entity, "test_scene");
    auto& scene_c = registry.emplace<SceneComp>(scene_entity);
    scene_c.json_data = R"(
    {
        "spawned_entity": {
            "PhysicsComp": {},
            "RenderComp": {}
        }
    }
    )"_json;

    // Watch entity
    entt::entity watch_entity = registry.create();
    auto& watch_c = registry.emplace<WatchComp>(watch_entity);
    // We test stopwatch updates separately, so we set the trigger as ready to go
    watch_c.end_reached = true;

    // Trigger entity
    entt::entity trigger_entity = registry.create();
    registry.emplace<WatchTriggerFlag>(trigger_entity);
    auto& pair_c = registry.emplace<PairComp>(trigger_entity);
    pair_c.source_entity = watch_entity;
    pair_c.target_entity = scene_entity;

    // Ensure that the scene entity is valid
    EXPECT_TRUE(registry.valid(pair_c.target_entity));

    // Ensure that the spawned entity does not exist yet
    EntityMap entity_map = entity_system.getEntityMap(registry);
    EXPECT_EQ(entity_map.find("spawned_entity"), entity_map.end());

    // Trigger
    scene_system.checkSceneTriggers(registry);

    // Check that the entities were created properly
    entity_map = entity_system.getEntityMap(registry);
    entt::entity spawned_entity = entity_map.at("spawned_entity");
    EXPECT_FALSE(spawned_entity == entt::null);
    EXPECT_EQ(registry.get<EntityName>(spawned_entity), "spawned_entity");
    EXPECT_TRUE(registry.valid(spawned_entity));
    auto& pc = registry.get<PhysicsComp>(spawned_entity);
    EXPECT_FLOAT_EQ(pc.mass, 1.0f);
}

TEST(SystemEntityTest, SyncPosition) {
    entt::registry registry;
    EntitySystem entity_system;

    // Create test entities
    entt::entity entity1 = registry.create();
    registry.emplace<EntityName>(entity1, "entity1");
    registry.emplace<PhysicsComp>(entity1, 1.0f, sf::Vector2f(10.f, 10.f));
    // Entity 2 will be the target synced with the source, entity 1
    entt::entity entity2 = registry.create();
    registry.emplace<EntityName>(entity2, "entity2");
    registry.emplace<PhysicsComp>(entity2);
    entt::entity rel_entity = registry.create();
    registry.emplace<PairComp>(rel_entity, entity2, entity1);
    registry.emplace<SyncPositionComp>(rel_entity);

    // Sync positions
    entity_system.syncEntities(registry);

    // Check that the positions are the same
    auto& pc1 = registry.get<PhysicsComp>(entity1);
    auto& pc2 = registry.get<PhysicsComp>(entity2);
    EXPECT_EQ(pc1.pos.x, pc2.pos.x);
    EXPECT_EQ(pc1.pos.y, pc2.pos.y);
}

// TODO: Implement or fulluy deprecate this.
// TEST(SystemEntityTest, ResolveEntityPairs) {
//     // Initialize the game in its test state
//     Game game("../../scenes/main_scene.json");
//     auto& registry = game.getRegistry();
//     auto& entity_system = game.getEntitySystem();
// 
//     // Resolve entity names
//     entity_system.resolvePairNames(registry);
// 
//     // Check that the entities match up
//     auto entity_map = game.getEntityMap();
//     auto rview = registry.view<EntityName, PairComp>();
//     for (auto [pair_entity, pair_entity_name, pair_c] : rview.each()) {
//         if (pair_entity_name == "player-beacon force") {
//             EXPECT_EQ(entity_map.at("player"), pair_c.target_entity);
//             EXPECT_EQ(entity_map.at("beacon"), pair_c.source_entity);
//         }
//     }
// }
// 
// TEST(SystemEntityTest, ResolveEntityPairsDelete) {
//     // Set up test objects
//     entt::registry registry;
//     EntitySystem entity_system;
//     PhysicsSystem physics_system;
//     // Entities themselves
//     entt::entity entity1 = registry.create();
//     registry.emplace<EntityName>(entity1, "entity1");
//     registry.emplace<PhysicsComp>(entity1);
//     entt::entity entity2 = registry.create();
//     registry.emplace<EntityName>(entity2, "entity2");
//     registry.emplace<PhysicsComp>(entity2);
//     entt::entity entity3 = registry.create();
//     registry.emplace<EntityName>(entity3, "entity3");
//     registry.emplace<PhysicsComp>(entity3);
//     // Entity-entity relationships
//     entt::entity rel_12 = registry.create();
//     PairComp pair_c_12;
//     pair_c_12.target_entity_name = "entity1";
//     pair_c_12.source_entity_name = "entity2";
//     registry.emplace<PairComp>(rel_12, pair_c_12);
//     registry.emplace<PairwiseForceComp>(rel_12);
//     entt::entity rel_23 = registry.create();
//     PairComp pair_c_23;
//     pair_c_23.target_entity_name = "entity2";
//     pair_c_23.source_entity_name = "entity3";
//     registry.emplace<PairComp>(rel_23, pair_c_23);
//     registry.emplace<PairwiseForceComp>(rel_23);
//     entt::entity rel_31 = registry.create();
//     PairComp pair_c_31;
//     pair_c_31.target_entity_name = "entity3";
//     pair_c_31.source_entity_name = "entity1";
//     registry.emplace<PairComp>(rel_31, pair_c_31);
//     registry.emplace<PairwiseForceComp>(rel_31);
// 
//     // Resolve names then forces
//     entity_system.getEntityMap(registry);
//     entity_system.resolvePairNames(registry);
//     EXPECT_TRUE(registry.valid(rel_12));
//     EXPECT_TRUE(registry.valid(rel_23));
//     EXPECT_TRUE(registry.valid(rel_31));
//     PairComp pair_c_12_now = registry.get<PairComp>(rel_12);
//     PairComp pair_c_23_now = registry.get<PairComp>(rel_23);
//     PairComp pair_c_31_now = registry.get<PairComp>(rel_31);
//     EXPECT_EQ(pair_c_12_now.target_entity, entity1);
//     EXPECT_EQ(pair_c_12_now.source_entity, entity2);
//     EXPECT_EQ(pair_c_23_now.target_entity, entity2);
//     EXPECT_EQ(pair_c_23_now.source_entity, entity3);
//     EXPECT_EQ(pair_c_31_now.target_entity, entity3);
//     EXPECT_EQ(pair_c_31_now.source_entity, entity1);
// 
//     // Delete an entity and try again
//     registry.destroy(entity1);
//     entity_system.getEntityMap(registry);
//     entity_system.resolvePairNames(registry);
//     EXPECT_FALSE(registry.valid(rel_12));
//     EXPECT_TRUE(registry.valid(rel_23));
//     EXPECT_FALSE(registry.valid(rel_31));
//     PairComp pair_c_23_final = registry.get<PairComp>(rel_23);
//     EXPECT_EQ(pair_c_23_final.target_entity, entity2);
//     EXPECT_EQ(pair_c_23_final.source_entity, entity3);
// }
// 
TEST(SystemPhysicsTest, CalculateForces) {
    // Initialize the game in its test state
    Game game("../../scenes/main_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Calculate forces
    physics_system.calculateForces(registry);
}

TEST(SystemPhysicsTest, CalculatePairwiseForces) {
    // Initialize the game in its test state
    Game game("../../scenes/main_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();
    // Need to resolve entity names before calculating pairwise forces

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
    physics_system.calculatePairwiseForces(registry);

    // Delete an entity and try again
    registry.destroy(entity1);
    physics_system.calculatePairwiseForces(registry);
}

TEST(SystemPhysicsTest, Update) {
    // Initialize the game in its test state
    Game game("../../scenes/main_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Update state
    physics_system.update(registry);
}

TEST(SystemPhysicsTest, UpdateStopWatches) {
    // Initialize the game in its test state
    Game game("../../scenes/main_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Update stop watches
    physics_system.updateStopWatches(registry);
}

TEST(SystemPhysicsTest, ResolveCollisions) {
    // Initialize the game in its test state
    Game game("../../scenes/main_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Resolve collisions
    physics_system.resolveCollisions(registry);
}

TEST(SystemPhysicsTest, UpdateDurability) {
    // Initialize the game in its test state
    Game game("../../scenes/main_scene.json");
    auto& registry = game.getRegistry();
    auto& physics_system = game.getPhysicsSystem();

    // Update durability
    physics_system.updateDurability(registry);
}

TEST(SystemRenderTest, Render) {
    // Initialize the game in its test state
    Game game("../../scenes/main_scene.json");
    auto& registry = game.getRegistry();
    auto& render_system = game.getRenderSystem();
    auto& window = game.getWindow();

    // Render
    render_system.render(registry, window);
}

TEST(SystemRenderTest, RenderUI) {
    // Initialize the game in its test state
    Game game("../../scenes/main_scene.json");
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
    Game game("../../scenes/main_scene.json");
    auto& registry = game.getRegistry();
    auto& render_system = game.getRenderSystem();
    auto& window = game.getWindow();

    // Render
    render_system.setView(registry, window, game.getView());
}