#include "config.h"
#include "component.h"
#include "scene.h"

#include <regex>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

using namespace twirl;

TEST(SceneTest, LoadFromJson) {
    // Create a Scene object
    SceneSystem scene_system;
    entt::registry registry;
    entt::entity scene = registry.create();
    registry.emplace<SceneComp>(scene, "../../tests/test_data/test_scene.json");

    // Main functions to test
    scene_system.loadJsonData(registry);

    // Loop through the registry and check if the components are added correctly
    auto rview = registry.view<MetadataComp>();
    ASSERT_FALSE(rview.empty());
    for (auto [entity, mc] : rview.each()) {
        // Check that the name mappings are correct
        EXPECT_EQ(entity, scene_system.name_to_entity_map.at(mc.name));

        if (mc.name == "player") {
            auto& pc = scene_system.registry.get<PhysicsComp>(entity);
            EXPECT_FLOAT_EQ(pc.mass, 10.0f);
            EXPECT_FLOAT_EQ(pc.pos.x, 1.0f * cfg.L);
            EXPECT_FLOAT_EQ(pc.pos.y, 1.0f * cfg.L);
            EXPECT_FLOAT_EQ(pc.vel.x, 2.0f * cfg.V);
            EXPECT_FLOAT_EQ(pc.vel.y, 2.0f * cfg.V);
            EXPECT_FLOAT_EQ(pc.force.x, 0.0f);
            EXPECT_FLOAT_EQ(pc.force.y, 0.0f);

            auto& dfc = scene_system.registry.get<DragForceComp>(entity);
            EXPECT_FLOAT_EQ(dfc.drag_coefficient, 0.05f);
            EXPECT_FLOAT_EQ(dfc.drag_power, 2.5f);

            auto& dc = scene_system.registry.get<DurabilityComp>(entity);
            EXPECT_FLOAT_EQ(dc.durability, 1.0f);
            EXPECT_FLOAT_EQ(dc.durability_loss_per_collision, 0.34f);
            EXPECT_FLOAT_EQ(dc.durability_regen_rate, 0.0f);
            EXPECT_EQ(dc.delete_at_zero, false);

            auto& rc = scene_system.registry.get<RenderComp>(entity);
            EXPECT_FLOAT_EQ(rc.shape.getRadius(), cfg.L);
            auto fill_color = rc.shape.getFillColor();
            EXPECT_EQ(fill_color.r, 255);
            EXPECT_EQ(fill_color.g, 128);
            EXPECT_EQ(fill_color.b, 128);
            EXPECT_EQ(fill_color.a, 255);

            auto& swc = scene_system.registry.get<StopWatchComp>(entity);
            EXPECT_FLOAT_EQ(swc.current_time, 0.0f);
            EXPECT_FLOAT_EQ(swc.end_time, 1.0f);
            EXPECT_EQ(swc.end_reached, false);
        } else if (mc.name == "beacon") {
            auto& pc = scene_system.registry.get<PhysicsComp>(entity);
            EXPECT_FLOAT_EQ(pc.mass, 1.0f);
            EXPECT_FLOAT_EQ(pc.pos.x, 0.0f);
            EXPECT_FLOAT_EQ(pc.pos.y, 0.0f);
            EXPECT_FLOAT_EQ(pc.vel.x, 0.0f);
            EXPECT_FLOAT_EQ(pc.vel.y, 0.0f);
            EXPECT_FLOAT_EQ(pc.force.x, 0.0f);
            EXPECT_FLOAT_EQ(pc.force.y, 0.0f);

            auto& rc = scene_system.registry.get<RenderComp>(entity);
            EXPECT_FLOAT_EQ(rc.shape.getRadius(), cfg.L / 2.f);
            EXPECT_FLOAT_EQ(rc.shape.getOutlineThickness(), cfg.L / 10.f);
            auto outline_color = rc.shape.getOutlineColor();
            EXPECT_EQ(outline_color.r, 255);
            EXPECT_EQ(outline_color.g, 255);
            EXPECT_EQ(outline_color.b, 255);
            EXPECT_EQ(outline_color.a, 255);

        } else if (mc.name == "player-beacon force") {
            auto& prc = scene_system.registry.get<PairComp>(entity);
            EXPECT_EQ(prc.target_entity, scene_system.name_to_entity_map.at("player"));
            EXPECT_EQ(prc.source_entity, scene_system.name_to_entity_map.at("beacon"));

            auto& pfc = scene_system.registry.get<PairwiseForceComp>(entity);
            EXPECT_FLOAT_EQ(pfc.magnitude, -1.0f);
            EXPECT_FLOAT_EQ(pfc.softening, 0.0f);
            EXPECT_FLOAT_EQ(pfc.power, 2.0f);
            EXPECT_FLOAT_EQ(pfc.min_distance, 0.1f);
            EXPECT_FLOAT_EQ(pfc.distance_scaling, 1.0f);
        } else if (std::regex_match(mc.name, std::regex("bkgrd.*"))) {
            // Background circle
            auto& rc = scene_system.registry.get<RenderComp>(entity);

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