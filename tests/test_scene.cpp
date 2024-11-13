#include "config.h"
#include "component.h"
#include "scene.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

using namespace twirl;

TEST(SceneTest, LoadFromJson) {
    // Create a Scene object
    Scene scene;

    // Main function to test
    scene.loadFromJson("../../tests/test_data/test_scene.json");

    // Loop through the registry and check if the components are added correctly
    auto rview = scene.registry.view<MetadataComp>();
    ASSERT_FALSE(rview.empty());
    for (auto [entity, mc] : rview.each()) {

        if (mc.name == "player") {
            auto& pc = scene.registry.get<PhysicsComp>(entity);
            EXPECT_FLOAT_EQ(pc.mass, 10.0f);
            EXPECT_FLOAT_EQ(pc.pos.x, 1.0f);
            EXPECT_FLOAT_EQ(pc.pos.y, 1.0f);
            EXPECT_FLOAT_EQ(pc.vel.x, 2.0f);
            EXPECT_FLOAT_EQ(pc.vel.y, 2.0f);
            EXPECT_FLOAT_EQ(pc.force.x, 0.0f);
            EXPECT_FLOAT_EQ(pc.force.y, 0.0f);

            auto& dfc = scene.registry.get<DragForceComp>(entity);
            EXPECT_FLOAT_EQ(dfc.drag_coefficient, 0.05f);
            EXPECT_FLOAT_EQ(dfc.drag_power, 2.5f);

            auto& dc = scene.registry.get<DurabilityComp>(entity);
            EXPECT_FLOAT_EQ(dc.durability, 1.0f);
            EXPECT_FLOAT_EQ(dc.durability_loss_per_collision, 0.34f);
            EXPECT_FLOAT_EQ(dc.durability_regen_rate, 0.0f);
            EXPECT_EQ(dc.delete_at_zero, false);

            auto& rc = scene.registry.get<RenderComp>(entity);
            EXPECT_FLOAT_EQ(rc.shape.getRadius(), cfg.L);
            auto fill_color = rc.shape.getFillColor();
            EXPECT_EQ(fill_color.r, 255);
            EXPECT_EQ(fill_color.g, 128);
            EXPECT_EQ(fill_color.b, 128);
            EXPECT_EQ(fill_color.a, 255);

            auto& sc = scene.registry.get<SpawnComp>(entity);
            EXPECT_EQ(sizeof(sc), sizeof(SpawnComp));

            auto& swc = scene.registry.get<StopWatchComp>(entity);
            EXPECT_FLOAT_EQ(swc.current_time, 0.0f);
            EXPECT_FLOAT_EQ(swc.end_time, 1.0f);
            EXPECT_EQ(swc.end_reached, false);
        } else if (mc.name == "beacon") {
            // Assert that MouseButtonReleasedComp is an empty struct
            auto& mbrc = scene.registry.get<MouseButtonReleasedComp>(entity);
            EXPECT_EQ(sizeof(mbrc), sizeof(MouseButtonReleasedComp));
        } else {
            FAIL() << "Unexpected entity name: " << mc.name;
        }
    }

}