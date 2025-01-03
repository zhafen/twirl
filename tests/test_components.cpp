
#include <regex>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>

#include "components/components.h"
#include "config.h"
#include "systems/system.h"

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
    EXPECT_EQ(registry.get<EntityName>(entity1), "entity1");
    EXPECT_EQ(registry.get<EntityName>(entity2), "entity2");

    // Delete the first entity and check that referencing still works
    registry.destroy(entity1);
    EXPECT_FALSE(registry.valid(entity1));
    EXPECT_EQ(registry.get<EntityName>(entity2), "entity2");

    // Check that the only EntityName component is entity2
    for (auto [entity, name] : registry.view<EntityName>().each()) {
        EXPECT_EQ(name, "entity2");
    }

    // Check that the only PhysicsComp component is entity2
    for (auto [entity, phys_c] : registry.view<PhysicsComp>().each()) {
        EXPECT_FLOAT_EQ(phys_c.mass, 2.0f);
    }
}

TEST(ComponentTest, WatchComp) {

    // Set up a registry and add some components
    entt::registry registry;

    // Check that the components were added correctly
    entt::entity entity1 = registry.create();
    registry.emplace<WatchComp>(entity1);
    EXPECT_FLOAT_EQ(registry.get<WatchComp>(entity1).current_time, 0.0f);
    EXPECT_FLOAT_EQ(registry.get<WatchComp>(entity1).end_time, 1.0f);
    EXPECT_EQ(registry.get<WatchComp>(entity1).end_reached, false);

    // Add another entity but with different defaults
    entt::entity entity2 = registry.create();
    registry.emplace<WatchComp>(entity2, 1.0f, 2.0f, true);
    EXPECT_FLOAT_EQ(registry.get<WatchComp>(entity2).current_time, 1.0f);
    EXPECT_FLOAT_EQ(registry.get<WatchComp>(entity2).end_time, 2.0f);
    EXPECT_EQ(registry.get<WatchComp>(entity2).end_reached, true);
}

TEST(ComponentTest, StopWatchCompJson) {

    entt::registry registry;

    // Load from json with no settings specified
    entt::entity entity1 = registry.create();
    json comp_json1 = R"({})"_json;
    comp::emplaceComponent(registry, entity1, "WatchComp", comp_json1);
    EXPECT_FLOAT_EQ(registry.get<WatchComp>(entity1).current_time, 0.0f);
    EXPECT_FLOAT_EQ(registry.get<WatchComp>(entity1).end_time, 1.0f);
    EXPECT_EQ(registry.get<WatchComp>(entity1).end_reached, false);

    // Load from json with all settings specified
    entt::entity entity2 = registry.create();
    json comp_json2 = R"(
    {
        "current_time": 1.0,
        "end_time": 2.0,
        "end_reached": true
    }
    )"_json;
    comp::emplaceComponent(registry, entity2, "WatchComp", comp_json2);
    EXPECT_FLOAT_EQ(registry.get<WatchComp>(entity2).current_time, 1.0f);
    EXPECT_FLOAT_EQ(registry.get<WatchComp>(entity2).end_time, 2.0f);
    EXPECT_EQ(registry.get<WatchComp>(entity2).end_reached, true);
}

TEST(ComponentTest, PairCompTest) {

    // Set up a registry and add some components
    entt::registry registry;

    // Check that the components were added correctly
    entt::entity rel_empty = registry.create();
    registry.emplace<PairComp>(rel_empty);
    EXPECT_TRUE(registry.get<PairComp>(rel_empty).target_entity == entt::null);
    EXPECT_TRUE(registry.get<PairComp>(rel_empty).source_entity == entt::null);
    EXPECT_TRUE(registry.get<PairComp>(rel_empty).target_entity_name == "");
    EXPECT_TRUE(registry.get<PairComp>(rel_empty).source_entity_name == "");

    // Add another entity but with different defaults
    entt::entity rel_12 = registry.create();
    entt::entity entity1 = registry.create();
    entt::entity entity2 = registry.create();
    registry.emplace<PairComp>(rel_12, entity1, entity2);
    EXPECT_TRUE(registry.get<PairComp>(rel_12).target_entity == entity1);
    EXPECT_TRUE(registry.get<PairComp>(rel_12).source_entity == entity2);
}

TEST(ComponentTest, PairCompJson) {

    entt::registry registry;

    // Check that the components were added correctly
    json comp_json_empty = R"({})"_json;
    entt::entity rel_empty = registry.create();
    comp::emplaceComponent(registry, rel_empty, "PairComp", comp_json_empty);
    EXPECT_TRUE(registry.get<PairComp>(rel_empty).target_entity == entt::null);
    EXPECT_TRUE(registry.get<PairComp>(rel_empty).source_entity == entt::null);
    EXPECT_TRUE(registry.get<PairComp>(rel_empty).target_entity_name == "");
    EXPECT_TRUE(registry.get<PairComp>(rel_empty).source_entity_name == "");

    // Add another entity but with different defaults
    json comp_json = R"(
    {
        "target_entity_name": "entity1",
        "source_entity_name": "entity2"
    }
    )"_json;
    entt::entity rel_12 = registry.create();
    comp::emplaceComponent(registry, rel_12, "PairComp", comp_json);
    EXPECT_TRUE(registry.get<PairComp>(rel_12).target_entity == entt::null);
    EXPECT_TRUE(registry.get<PairComp>(rel_12).source_entity == entt::null);
    EXPECT_TRUE(registry.get<PairComp>(rel_12).target_entity_name == "entity1");
    EXPECT_TRUE(registry.get<PairComp>(rel_12).source_entity_name == "entity2");
}

TEST(ComponentTest, EntityFromCompStr) {
    entt::registry registry;

    // Create an entity with a component
    entt::entity entity = registry.create();
    registry.emplace<EnemyFlag>(entity);
    registry.emplace<PhysicsComp>(entity, 10.0f);

    // Check that the entity can be retrieved from the component string
    entt::entity entity_out = comp::getEntityFromStr(registry, "EnemyFlag|first");
    EXPECT_EQ(entity, entity_out);

    // Check that the entity can be retrieved from the component string
    entt::entity entity_out2 = comp::getEntityFromStr(registry, "PhysicsComp|first");
    EXPECT_EQ(entity, entity_out2);
}