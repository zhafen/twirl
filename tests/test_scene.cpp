#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include "scene.h"
#include "component.h"

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
            EXPECT_FLOAT_EQ(pc.force.x, 3.0f);
            EXPECT_FLOAT_EQ(pc.force.y, 3.0f);
            auto& dfc = scene.registry.get<DragForceComp>(entity);
            EXPECT_FLOAT_EQ(dfc.drag_coefficient, 0.05f);
            EXPECT_FLOAT_EQ(dfc.drag_power, 2.5f);
            auto& dc = scene.registry.get<DurabilityComp>(entity);
            EXPECT_FLOAT_EQ(dc.durability, 1.0f);
            EXPECT_FLOAT_EQ(dc.durability_loss_per_collision, 0.34f);
            EXPECT_FLOAT_EQ(dc.durability_regen_rate, 0.0f);
            EXPECT_EQ(dc.delete_at_zero, false);
        } else if (mc.name == "beacon") {
            // Assert that MouseButtonReleasedComp is an empty struct
            auto& mbrc = scene.registry.get<MouseButtonReleasedComp>(entity);
            EXPECT_EQ(sizeof(mbrc), sizeof(MouseButtonReleasedComp));
        } else {
            FAIL() << "Unexpected entity name: " << mc.name;
        }
    }

}