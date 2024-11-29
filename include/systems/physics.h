#ifndef PHYSICS_H
#define PHYSICS_H

#include "systems/render.h"

#include <functional>
#include <memory>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "component.h"
#include "config.h"
#include "shape.h"

namespace twirl {

class PhysicsSystem {
   public:
    PhysicsSystem() = default;
    void calculateForces(entt::registry& registry);
    void calculatePairwiseForces(entt::registry& registry);
    void update(entt::registry& registry);
    void updateStopWatches(entt::registry& registry);
    void resolveCollisions(entt::registry& registry);
    void updateDurability(entt::registry& registry);
};

} // namespace twirl

#endif  // PHYSICS_H