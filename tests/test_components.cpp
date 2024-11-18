
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
    for (auto [entity, pc] : registry.view<PhysicsComp>().each()) {
        ASSERT_FLOAT_EQ(pc.mass, 2.0f);
    }
}