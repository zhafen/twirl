#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>
#include <regex>

#include "components/components.h"
#include "config.h"
#include "systems/system.h"

using namespace twirl;

TEST(SceneTest, TriggerScene) {
    // Set up registry and scene system, including the trigger
    SceneSystem scene_system;
    entt::registry registry;
    registry.on_update<SceneTriggerComp>().connect<&SceneSystem::onSceneTrigger>(
        scene_system);

    // Add a scene to the registry (including some manually-input json data)
    entt::entity scene = registry.create();
    registry.emplace<EntityName>(scene, "test_scene");
    json json_data = R"(
    {
        "spawned_entity": {"components": {"EnemyComp": {}}}
    }
    )"_json;
    SceneComp& scene_c = registry.emplace<SceneComp>(scene);
    scene_c.json_data = json_data;

    // Add the triggering entity
    entt::entity triggering_entity = registry.create();
    registry.emplace<EntityName>(triggering_entity, "triggering_entity");
    registry.emplace<SceneTriggerComp>(triggering_entity, "test_scene", scene);

    // Trigger the scene
    bool is_valid = registry.all_of<SceneTriggerComp>(triggering_entity);
    registry.patch<SceneTriggerComp>(
        triggering_entity, [](auto& scenetrigger_c) { scenetrigger_c.n_triggers++; });

    // Check that the entity was added
    auto rview = registry.view<EnemyComp>();
    ASSERT_FALSE(rview.empty());
}

// This test checks that the initial values for entities loaded from json
// that require their names to be resolved are null
TEST(SceneTest, EmplaceEntityUnresovledNames) {
    entt::registry registry;
    SceneSystem scene_system;
    EntitySystem entity_system;

    // For SceneTriggerComp
    json entity1_json = R"(
    {
        "components": {
            "SceneTriggerComp": {
                "scene_name": "test_scene"
            }
        }
    }
    )"_json;
    scene_system.emplaceEntity(registry, "entity1", entity1_json);
    // For PairComp
    json rel_12_json = R"(
    {
        "components": {
            "PairComp": {
                "target_entity_name": "entity1",
                "source_entity_name": "entity2"
            }
        }
    }
    )"_json;
    scene_system.emplaceEntity(registry, "rel_12", rel_12_json);

    // Resolve the entity names and check the values
    // We start by getting the map
    EntityMap entity_map = entity_system.getEntityMap(registry);

    // Check that SceneTriggerComp got emplaced correctly
    entt::entity entity1 = entity_map.at("entity1");
    auto scenetrigger_c = registry.get<SceneTriggerComp>(entity1);
    ASSERT_TRUE(scenetrigger_c.scene_name == "test_scene");
    ASSERT_TRUE(scenetrigger_c.scene_entity == entt::null);

    // Check that PairComp got emplaced correctly
    entt::entity rel_12 = entity_map.at("rel_12");
    auto pair_c = registry.get<PairComp>(rel_12);
    ASSERT_TRUE(pair_c.target_entity_name == "entity1");
    ASSERT_TRUE(pair_c.source_entity_name == "entity2");
    ASSERT_TRUE(pair_c.target_entity == entt::null);
    ASSERT_TRUE(pair_c.source_entity == entt::null);
}

TEST(SceneTest, EmplaceSceneFromJson) {
    // Set up registry and scene system, including the trigger
    SceneSystem scene_system;
    entt::registry registry;

    // Add a scene to the registry (including some manually-input json data)
    entt::entity scene = registry.create();
    auto& scene_c =
        registry.emplace<SceneComp>(scene, "../../tests/test_data/test_scene.json");
    scene_c.verbose_names = false;
    registry.emplace<EntityName>(scene, "test_scene");
    scene_system.loadJsonData(registry);

    // Emplace the Scene
    // DEBUG
    bool is_valid = registry.valid(scene);
    scene_system.emplaceScene(registry, scene);

    // Loop through the registry and check if the components are added correctly
    auto rview = registry.view<EntityName>();
    ASSERT_FALSE(rview.empty());
    for (auto [entity, name] : rview.each()) {
        // Check that the name mappings are correct
        EXPECT_EQ(entity, entt::to_entity(registry, name));

        if (name == "player") {
            auto& pc = registry.get<PhysicsComp>(entity);
            EXPECT_FLOAT_EQ(pc.mass, 1.0f);
            EXPECT_FLOAT_EQ(pc.pos.x, 1.0f * cfg.L);
            EXPECT_FLOAT_EQ(pc.pos.y, 1.0f * cfg.L);
            EXPECT_FLOAT_EQ(pc.vel.x, 2.0f * cfg.V);
            EXPECT_FLOAT_EQ(pc.vel.y, 2.0f * cfg.V);
            EXPECT_FLOAT_EQ(pc.force.x, 0.0f);
            EXPECT_FLOAT_EQ(pc.force.y, 0.0f);

            auto& dfc = registry.get<DragForceComp>(entity);
            EXPECT_FLOAT_EQ(dfc.drag_coefficient, 0.05f);
            EXPECT_FLOAT_EQ(dfc.drag_power, 2.5f);

            auto& dc = registry.get<DurabilityComp>(entity);
            EXPECT_FLOAT_EQ(dc.durability, 1.0f);
            EXPECT_FLOAT_EQ(dc.durability_loss_per_collision, 0.34f);
            EXPECT_FLOAT_EQ(dc.durability_regen_rate, 0.0f);
            EXPECT_EQ(dc.delete_at_zero, false);

            auto& rc = registry.get<RenderComp>(entity);
            EXPECT_FLOAT_EQ(rc.shape.getRadius(), cfg.L);
            auto fill_color = rc.shape.getFillColor();
            EXPECT_EQ(fill_color.r, 255);
            EXPECT_EQ(fill_color.g, 128);
            EXPECT_EQ(fill_color.b, 128);
            EXPECT_EQ(fill_color.a, 255);

            auto& swc = registry.get<StopWatchComp>(entity);
            EXPECT_FLOAT_EQ(swc.current_time, 0.0f);
            EXPECT_FLOAT_EQ(swc.end_time, 1.0f);
            EXPECT_EQ(swc.end_reached, false);
        } else if (name == "beacon") {
            auto& pc = registry.get<PhysicsComp>(entity);
            EXPECT_FLOAT_EQ(pc.mass, 1.0f);
            EXPECT_FLOAT_EQ(pc.pos.x, 0.0f);
            EXPECT_FLOAT_EQ(pc.pos.y, 0.0f);
            EXPECT_FLOAT_EQ(pc.vel.x, 0.0f);
            EXPECT_FLOAT_EQ(pc.vel.y, 0.0f);
            EXPECT_FLOAT_EQ(pc.force.x, 0.0f);
            EXPECT_FLOAT_EQ(pc.force.y, 0.0f);

            auto& rc = registry.get<RenderComp>(entity);
            EXPECT_FLOAT_EQ(rc.shape.getRadius(), cfg.L / 2.f);
            EXPECT_FLOAT_EQ(rc.shape.getOutlineThickness(), cfg.L / 10.f);
            auto outline_color = rc.shape.getOutlineColor();
            EXPECT_EQ(outline_color.r, 255);
            EXPECT_EQ(outline_color.g, 255);
            EXPECT_EQ(outline_color.b, 255);
            EXPECT_EQ(outline_color.a, 255);

        } else if (name == "player-beacon force") {
            // We don't check for the correct entities themselves here because
            // that's part of resolvePairNames
            auto& prc = registry.get<PairComp>(entity);
            EXPECT_EQ(prc.target_entity_name, "player");
            EXPECT_EQ(prc.source_entity_name, "beacon");

            auto& pfc = registry.get<PairwiseForceComp>(entity);
            EXPECT_FLOAT_EQ(pfc.magnitude, -1.0f);
            EXPECT_FLOAT_EQ(pfc.softening, 0.0f);
            EXPECT_FLOAT_EQ(pfc.power, 2.0f);
            EXPECT_FLOAT_EQ(pfc.min_distance, 0.0f);
            EXPECT_FLOAT_EQ(pfc.distance_scaling, 1.0f);
        } else if (std::regex_match(name, std::regex("bkgrd.*"))) {
            // Background circle
            auto& rc = registry.get<RenderComp>(entity);

            auto fill_color = rc.shape.getFillColor();
            EXPECT_EQ(fill_color.r, 127);
            EXPECT_EQ(fill_color.g, 127);
            EXPECT_EQ(fill_color.b, 127);
            EXPECT_EQ(fill_color.a, 255);

            auto outline_color = rc.shape.getOutlineColor();
            EXPECT_EQ(outline_color.r, 63);
            EXPECT_EQ(outline_color.g, 63);
            EXPECT_EQ(outline_color.b, 63);
            EXPECT_EQ(outline_color.a, 255);
        }
    }
}