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
    scene.loadFromJson("tests/test_data/test_scene.json");

    // Loop through the registry and check if the components are added correctly
    auto rview = scene.registry.view<MetadataComp>();
    ASSERT_FALSE(rview.empty());
    for (auto [entity, mc] : rview.each()) {

        if (mc.name == "Player") {
            auto& dfc = scene.registry.get<DragForceComp>(entity);
            EXPECT_EQ(dfc.drag_coefficient, 0.05f);
        } else {
            FAIL() << "Unexpected entity name: " << mc.name;
        }
    }

}