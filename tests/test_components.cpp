
#include <regex>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>

#include "component.h"
#include "config.h"
#include "scene.h"

using namespace twirl;

TEST(ComponentTest, EntityNameStorage) {

    // Set up a registry and add some components
    entt::registry registry;
    entt::entity entity1 = registry.create();
    registry.emplace<EntityName>(entity1, "entity1");
    registry.emplace<PhysicsComp>(entity1, 1.0f);
    entt::entity entity2 = registry.create();
    registry.emplace<EntityName>(entity2, "entity2");
    registry.emplace<PhysicsComp>(entity2, 2.0f);

    // Check that referencing works prior to any deletion
    ASSERT_EQ(registry.get<EntityName>(entity1), "entity1");
    ASSERT_EQ(registry.get<EntityName>(entity2), "entity2");

    // Delete the first entity and check that referencing still works
    registry.destroy(entity1);
    ASSERT_FALSE(registry.valid(entity1));
    ASSERT_EQ(registry.get<EntityName>(entity2), "entity2");

    // Check that the only EntityName component is entity2
    for (auto [entity, name] : registry.view<EntityName>().each()) {
        ASSERT_EQ(name, "entity2");
    }

    // Check that the only PhysicsComp component is entity2
    for (auto [entity, phys_c] : registry.view<PhysicsComp>().each()) {
        ASSERT_FLOAT_EQ(phys_c.mass, 2.0f);
    }
}

TEST(ComponentTest, StopWatchComp) {

    // Set up a registry and add some components
    entt::registry registry;

    // Check that the components were added correctly
    entt::entity entity1 = registry.create();
    registry.emplace<StopWatchComp>(entity1);
    ASSERT_FLOAT_EQ(registry.get<StopWatchComp>(entity1).current_time, 0.0f);
    ASSERT_FLOAT_EQ(registry.get<StopWatchComp>(entity1).end_time, 1.0f);
    ASSERT_EQ(registry.get<StopWatchComp>(entity1).end_reached, false);

    // Add another entity but with different defaults
    entt::entity entity2 = registry.create();
    registry.emplace<StopWatchComp>(entity2, 1.0f, 2.0f, true);
    ASSERT_FLOAT_EQ(registry.get<StopWatchComp>(entity2).current_time, 1.0f);
    ASSERT_FLOAT_EQ(registry.get<StopWatchComp>(entity2).end_time, 2.0f);
    ASSERT_EQ(registry.get<StopWatchComp>(entity2).end_reached, true);
}

TEST(ComponentTest, StopWatchCompJson) {

    entt::registry registry;
    SceneSystem scene_system;

    // Load from json with no settings specified
    entt::entity entity1 = registry.create();
    json comp_json1 = R"({})"_json;
    scene_system.emplaceComponent(registry, entity1, "StopWatchComp", comp_json1);
    ASSERT_FLOAT_EQ(registry.get<StopWatchComp>(entity1).current_time, 0.0f);
    ASSERT_FLOAT_EQ(registry.get<StopWatchComp>(entity1).end_time, 1.0f);
    ASSERT_EQ(registry.get<StopWatchComp>(entity1).end_reached, false);

    // Load from json with all settings specified
    entt::entity entity2 = registry.create();
    json comp_json2 = R"(
    {
        "current_time": 1.0,
        "end_time": 2.0,
        "end_reached": true
    }
    )"_json;
    scene_system.emplaceComponent(registry, entity2, "StopWatchComp", comp_json2);
    ASSERT_FLOAT_EQ(registry.get<StopWatchComp>(entity2).current_time, 1.0f);
    ASSERT_FLOAT_EQ(registry.get<StopWatchComp>(entity2).end_time, 2.0f);
    ASSERT_EQ(registry.get<StopWatchComp>(entity2).end_reached, true);
}

TEST(ComponentTest, PairCompTest) {

    // Set up a registry and add some components
    entt::registry registry;

    // Check that the components were added correctly
    entt::entity rel_empty = registry.create();
    registry.emplace<PairComp>(rel_empty);
    ASSERT_TRUE(registry.get<PairComp>(rel_empty).target_entity == entt::null);
    ASSERT_TRUE(registry.get<PairComp>(rel_empty).source_entity == entt::null);
    ASSERT_TRUE(registry.get<PairComp>(rel_empty).target_entity_name == "");
    ASSERT_TRUE(registry.get<PairComp>(rel_empty).source_entity_name == "");

    // Add another entity but with different defaults
    entt::entity rel_12 = registry.create();
    entt::entity entity1 = registry.create();
    entt::entity entity2 = registry.create();
    registry.emplace<PairComp>(rel_12, entity1, entity2);
    ASSERT_TRUE(registry.get<PairComp>(rel_12).target_entity == entity1);
    ASSERT_TRUE(registry.get<PairComp>(rel_12).source_entity == entity2);
}

TEST(ComponentTest, PairCompJson) {

    entt::registry registry;
    SceneSystem scene_system;

    // Check that the components were added correctly
    json comp_json_empty = R"({})"_json;
    entt::entity rel_empty = registry.create();
    scene_system.emplaceComponent(registry, rel_empty, "PairComp", comp_json_empty);
    ASSERT_TRUE(registry.get<PairComp>(rel_empty).target_entity == entt::null);
    ASSERT_TRUE(registry.get<PairComp>(rel_empty).source_entity == entt::null);
    ASSERT_TRUE(registry.get<PairComp>(rel_empty).target_entity_name == "");
    ASSERT_TRUE(registry.get<PairComp>(rel_empty).source_entity_name == "");

    // Add another entity but with different defaults
    json comp_json = R"(
    {
        "target_entity_name": "entity1",
        "source_entity_name": "entity2"
    }
    )"_json;
    entt::entity rel_12 = registry.create();
    scene_system.emplaceComponent(registry, rel_12, "PairComp", comp_json);
    ASSERT_TRUE(registry.get<PairComp>(rel_12).target_entity == entt::null);
    ASSERT_TRUE(registry.get<PairComp>(rel_12).source_entity == entt::null);
    ASSERT_TRUE(registry.get<PairComp>(rel_12).target_entity_name == "entity1");
    ASSERT_TRUE(registry.get<PairComp>(rel_12).source_entity_name == "entity2");
}