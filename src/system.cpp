#include "system.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <iostream>

#include "config.h"

namespace twirl {

EntityMap& EntitySystem::getEntityMap(entt::registry& registry) {

    // Clear the map to start
    entity_map.clear();

    auto rview = registry.view<EntityName>();
    for (auto [entity, name] : rview.each()) {
        entity_map.emplace(name, entity);
    }

    return entity_map;
}

entt::entity EntitySystem::resolveEntityName(entt::registry& registry, EntityName name,
                               entt::entity entity) {
    // If there's already a valid entity, return it
    if (registry.valid(entity)) {
        return entity;
    }

    // If the the name is not empty, try to resolve it
    if (!name.empty()) {
        // Try to get the entity name from the entity map
        if (entity_map.find(name) != entity_map.end()) {
            return entity_map.at(name);
        }
    }
    return entt::null;
}

void EntitySystem::resolveEntityNames(entt::registry& registry) {
    resolveSceneTriggerNames(registry);
    resolvePairNames(registry);
}

void EntitySystem::resolveSceneTriggerNames(entt::registry& registry) {
    auto rview = registry.view<SceneTriggerComp>();
    for (auto [entity, scenetrigger_c] : rview.each()) {
        scenetrigger_c.scene_entity = resolveEntityName(registry, scenetrigger_c.scene_name,
                                                        scenetrigger_c.scene_entity);
    }
}

/**
 * OPTIMIZE: This function may not be able to loop over every paircomp if we use
 * listeners, and should probably not call getEntityMap every time.
 */
void EntitySystem::resolvePairNames(entt::registry& registry) {
    auto rview = registry.view<PairComp>();
    for (auto [pair_entity, pair_c] : rview.each()) {
        pair_c.target_entity = resolveEntityName(registry, pair_c.target_entity_name,
                                                 pair_c.target_entity);
        pair_c.source_entity = resolveEntityName(registry, pair_c.source_entity_name,
                                                 pair_c.source_entity);
        // If either entity is not resolved after trying to resolve the names,
        // destroy the pair entity
        if ((pair_c.target_entity == entt::null) ||
            (pair_c.source_entity == entt::null)) {
            registry.destroy(pair_entity);
        }
    }
}

void EntitySystem::spawnEntities(entt::registry& registry) {
    auto rview = registry.view<SceneTriggerComp, StopWatchComp>();
    for (auto [entity, scenetrigger_c, stopwatch_c] : rview.each()) {
        // Check if the end time was reached
        if (!stopwatch_c.end_reached) {
            continue;
        } else {
            // Reset the stopwatch
            stopwatch_c.current_time = 0.0f;
            stopwatch_c.end_reached = false;
        }

        // If we got this far then we activate the scene trigger
        registry.patch<SceneTriggerComp>(entity, [](auto& stc) { stc.n_triggers++; });
    }
}

void EntitySystem::deleteEntities(entt::registry& registry) {
    auto rview = registry.view<DeleteComp>();

    for (auto entity : rview) {
        registry.destroy(entity);
        needs_ordering = true;
    }
}

void EntitySystem::orderEntities(entt::registry& registry) {
    if (!needs_ordering) {
        return;
    }

    registry.sort<RenderComp>(
        [](const auto lhs, const auto rhs) { return lhs.zorder < rhs.zorder; });

    // Mark that the entities are ordered
    needs_ordering = false;
}

/**
 * @brief Syncs entity values
 *
 * Currently I have to hardcode each combination of synced values.
 * Generalizing this requires reflection, which is perhaps easiest solved
 * by writing Python to generate the functions.
 *
 * @param registry The registry containing all entities and components.
 */
void EntitySystem::syncEntities(entt::registry& registry) {
    // Sync entity positions
    auto rview = registry.view<PairComp, SyncPositionComp>();
    for (auto [rel_entity, pair_c, syncpos_c] : rview.each()) {
        PhysicsComp& target_phys_c = registry.get<PhysicsComp>(pair_c.target_entity);
        PhysicsComp source_phys_c = registry.get<PhysicsComp>(pair_c.source_entity);
        target_phys_c.pos = source_phys_c.pos;

        // Destroy the relationship if it's a one-time sync
        if (syncpos_c.once_only) {
            registry.remove<SyncPositionComp>(rel_entity);
        }
    }
}

void PhysicsSystem::calculateForces(entt::registry& registry) {
    auto rview = registry.view<PhysicsComp, DragForceComp>();
    for (auto [entity, phys_c, dragforce_c] : rview.each()) {
        float vel_mag =
            sqrtf(phys_c.vel.x * phys_c.vel.x + phys_c.vel.y * phys_c.vel.y);
        sf::Vector2f vel_scaling =
            (phys_c.vel / cfg.V) * powf(vel_mag / cfg.V, dragforce_c.drag_power - 1.0f);
        phys_c.force -= cfg.A * dragforce_c.drag_coefficient * vel_scaling;
    }
}

void PhysicsSystem::calculatePairwiseForces(entt::registry& registry) {
    auto rview = registry.view<PairComp, PairwiseForceComp>();

    for (auto [rel_id, pair_c, pairforce_c] : rview.each()) {
        auto& target_phys_c = registry.get<PhysicsComp>(pair_c.target_entity);
        auto& source_phys_c = registry.get<PhysicsComp>(pair_c.source_entity);

        auto r = target_phys_c.pos - source_phys_c.pos;
        auto r_mag = sqrtf(r.x * r.x + r.y * r.y);

        // Don't calculate the distance when too close
        if (r_mag < pairforce_c.min_distance * cfg.L) {
            continue;
        }

        auto r_hat = r / r_mag;
        auto r_mag_scaled =
            (r_mag / cfg.L + pairforce_c.softening) / pairforce_c.distance_scaling;
        auto force = cfg.A * r_hat * pairforce_c.magnitude * target_phys_c.mass *
                     source_phys_c.mass * powf(r_mag_scaled, pairforce_c.power);

        target_phys_c.force += force;
    }
}

void PhysicsSystem::update(entt::registry& registry) {
    auto rview = registry.view<PhysicsComp>();

    for (auto [entity, phys_c] : rview.each()) {
        // Update using leapfrog algorithm
        auto acc = phys_c.force / phys_c.mass;
        phys_c.vel += acc * cfg.dt / 2.f;
        phys_c.pos += phys_c.vel * cfg.dt;
        phys_c.vel += acc * cfg.dt / 2.f;

        // Reset force
        phys_c.force = {0.f, 0.f};
    }
}

void PhysicsSystem::updateStopWatches(entt::registry& registry) {
    auto rview = registry.view<StopWatchComp>();
    for (auto [entity, stopwatch_c] : rview.each()) {
        // Keep going if the end time was already reached.
        if (stopwatch_c.end_reached) {
            continue;
        }

        stopwatch_c.current_time += cfg.dt;

        if (stopwatch_c.current_time >= stopwatch_c.end_time) {
            stopwatch_c.end_reached = true;
        }
    }
}

/**
 * @brief Resolves collisions between physics components.
 *
 * In an elastic collision, the momentum and kinetic energy are conserved.
 * These lead to the constraint that the center-of-mass velocity is constant.
 * In turn we can derive that in the center-of-mass frame the particle momentums
 * are equal and opposite, and there is a relationship $p^2/2\mu = T$ where $p$ is
 * the magnitude of the momentum in the COM frame, $\mu$ is the reduced mass
 * $\mu = m1 m2 / (m1 + m2)$, and $T$ is the kinetic energy.
 *
 * This is different from the common assumption that the velocities are
 * equal and opposite. Honestly I don't know where that assumption comes from,
 * but the above COM constraints can be derived easily.
 *
 * There is not a direct constraint from the energy and momentum conservation on the
 * angle of p1 and p2. However, we employ a simple assumption that the objects
 * move parallel to the line connecting their centers. If you look on wikipedia there
 * are more-thorough constraints based on contact angles.
 */
void PhysicsSystem::resolveCollisions(entt::registry& registry) {
    for (auto [rel_id, pair_c] : registry.view<PairComp, CollisionComp>().each()) {
        // Check if the entities still exist
        if (!registry.valid(pair_c.target_entity) ||
            !registry.valid(pair_c.source_entity)) {
            registry.destroy(rel_id);
            continue;
        }

        // Get first entity
        auto& entity1 = pair_c.target_entity;
        auto& phys_c1 = registry.get<PhysicsComp>(entity1);
        auto& rend_c1 = registry.get<RenderComp>(entity1);

        // Get second entity
        auto& entity2 = pair_c.source_entity;
        auto& phys_c2 = registry.get<PhysicsComp>(entity2);
        auto& rend_c2 = registry.get<RenderComp>(entity2);

        // Check for collision, assuming circular shapes for all objects that collide
        auto r_12 = phys_c2.pos - phys_c1.pos;
        auto r_12_mag = sqrtf(r_12.x * r_12.x + r_12.y * r_12.y);
        if (r_12_mag > rend_c1.shape.getRadius() + rend_c2.shape.getRadius()) {
            continue;
        }

        // Calculate the momentum in the COM frame
        auto T = 0.5f * (phys_c1.mass * (phys_c1.vel.x * phys_c1.vel.x +
                                         phys_c1.vel.y * phys_c1.vel.y) +
                         phys_c2.mass * (phys_c2.vel.x * phys_c2.vel.x +
                                         phys_c2.vel.y * phys_c2.vel.y));
        auto pcom_mag = sqrtf(2.0f * T * phys_c1.mass * phys_c2.mass /
                              (phys_c1.mass + phys_c2.mass));
        auto p1com = -pcom_mag * r_12 / r_12_mag;

        // Convert back to default frame
        auto vcom = (phys_c1.vel * phys_c1.mass + phys_c2.vel * phys_c2.mass) /
                    (phys_c1.mass + phys_c2.mass);
        phys_c1.vel = vcom + p1com / phys_c1.mass;
        phys_c2.vel = vcom - p1com / phys_c2.mass;

        // Indicate collision
        phys_c1.collided = true;
        phys_c2.collided = true;
    }
}

void PhysicsSystem::updateDurability(entt::registry& registry) {
    auto rview = registry.view<DurabilityComp, RenderComp, PhysicsComp>();
    for (auto [entity, dur_c, rend_c, phys_c] : rview.each()) {
        // Regenerate durability
        dur_c.durability += dur_c.durability_regen_rate * cfg.dt;

        // Apply durability loss from collision
        if (phys_c.collided) {
            dur_c.durability -= dur_c.durability_loss_per_collision;
            phys_c.collided = false;
        }

        // Cap durability at 1.0
        if (dur_c.durability > 1.0f) {
            dur_c.durability = 1.0f;
            rend_c.shape.setFillColor(sf::Color::White);
        } else if (dur_c.durability < 0.0f) {
            // When out of durability, set to 0 and change color
            dur_c.durability = 0.0f;
            if (dur_c.delete_at_zero) {
                registry.emplace<DeleteComp>(entity);
            } else {
                rend_c.shape.setFillColor(sf::Color(63, 63, 63));
            }
        }
    }
}

RenderSystem::RenderSystem(sf::View& view, sf::View& ui_view)
    : view(view), ui_view(ui_view) {}

void RenderSystem::render(entt::registry& registry, sf::RenderWindow& window) {
    window.clear(sf::Color::Black);

    // The .use ensures we use the order of render components
    auto rview = registry.view<RenderComp, PhysicsComp>().use<RenderComp>();

    // draw frame
    for (auto [entity, rend_c, phys_c] : rview.each()) {
        rend_c.shape.setPosition(phys_c.pos);
        window.draw(rend_c.shape);
    }
}

void RenderSystem::renderUI(entt::registry& registry, sf::RenderWindow& window) {
    window.setView(ui_view);

    // draw frame
    for (auto [entity, ui_c] : registry.view<UIComp>().each()) {
        // Get versions of the size and position that can be modified
        auto ui_c_size = ui_c.size;
        auto ui_c_pos = ui_c.pos;

        // Scale size with the tracked value
        ui_c_size.x *= *ui_c.tracked_value;
        // Causing the bar to shrink from the center requires changing both
        // the size and position
        ui_c_pos.x += (ui_c.size.x - ui_c_size.x) / 2.f;
        ui_c.shape.setSize(ui_c_size);
        ui_c.shape.setPosition(ui_c_pos);

        window.draw(ui_c.shape);
    }
}

void RenderSystem::setView(entt::registry& registry, sf::RenderWindow& window,
                           sf::View& view) {
    // Pin the view to the ViewComp entity (the player)
    auto rview = registry.view<PhysicsComp, ViewComp>();
    size_t n_vc = 0;
    for (auto [entity, phys_c] : rview.each()) {
        if (n_vc > 1) {
            std::cerr << "Warning: More than one ViewComp entity found. Using the "
                         "first one.\n";
            break;
        }
        view.setCenter(phys_c.pos);
        window.setView(view);
        n_vc++;
    }
}

}  // namespace twirl