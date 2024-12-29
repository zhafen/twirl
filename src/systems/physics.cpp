#include "systems/physics.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <iostream>

#include "components/components.h"
#include "config.h"
#include "shape.h"

namespace twirl {

// Drag force is a force of the form - F_typical * (v / v_max)^power
// v_max is roughly the maximum velocity of the entity given a force F_typical.
// The magnitude is F_typical.
void PhysicsSystem::calculateForces(entt::registry& registry) {
    auto rview = registry.view<PhysicsComp, DragForceComp>();
    for (auto [entity, phys_c, dragforce_c] : rview.each()) {
        float vel_mag =
            sqrtf(phys_c.vel.x * phys_c.vel.x + phys_c.vel.y * phys_c.vel.y);
        sf::Vector2f vel_scaling =
            (phys_c.vel / dragforce_c.terminal_velocity) *
            powf(vel_mag / dragforce_c.terminal_velocity, dragforce_c.power - 1.0f);
        phys_c.force -= dragforce_c.magnitude * vel_scaling;
    }
}

void PhysicsSystem::calculatePairwiseForces(entt::registry& registry) {
    auto rview = registry.view<PairComp, PairwiseForceComp>();

    for (auto [rel_id, pair_c, pairforce_c] : rview.each()) {
        // Keep going if the entities don't exist
        if ((!registry.valid(pair_c.target_entity)) ||
            (!registry.valid(pair_c.source_entity))) {
            registry.destroy(rel_id);
            continue;
        }

        auto& target_phys_c = registry.get<PhysicsComp>(pair_c.target_entity);
        auto& source_phys_c = registry.get<PhysicsComp>(pair_c.source_entity);

        auto r = target_phys_c.pos - source_phys_c.pos;
        auto r_mag = sqrtf(r.x * r.x + r.y * r.y);

        // Don't calculate the distance when too close
        if (r_mag < pairforce_c.min_distance) {
            continue;
        }

        auto r_hat = r / r_mag;
        auto r_mag_scaled =
            (r_mag + pairforce_c.softening) / pairforce_c.distance_scaling;
        auto force = r_hat * pairforce_c.magnitude * target_phys_c.mass *
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
    auto rview = registry.view<WatchComp>();
    for (auto [entity, stopwatch_c] : rview.each()) {
        // If the end time was reached, reset
        if (stopwatch_c.end_reached) {
            stopwatch_c.current_time = 0.0f;
            stopwatch_c.end_reached = false;
        }

        stopwatch_c.current_time += cfg.dt;

        // Check if the end time was reached,
        // and set end_reached to true until the next frame
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
    for (auto [rel_id, pair_c, collision_c] :
         registry.view<PairComp, CollisionComp>().each()) {
        // Check if the entities still exist
        if (!registry.valid(pair_c.target_entity) ||
            !registry.valid(pair_c.source_entity)) {
            registry.destroy(rel_id);
            continue;
        }

        // Get first entity
        auto& entity1 = pair_c.target_entity;
        auto& phys_c1 = registry.get<PhysicsComp>(entity1);
        auto& rend_c1 = registry.get<CircleComp>(entity1);

        // Get second entity
        auto& entity2 = pair_c.source_entity;
        auto& phys_c2 = registry.get<PhysicsComp>(entity2);
        auto& rend_c2 = registry.get<CircleComp>(entity2);

        // Check for collision, assuming circular shapes for all objects that collide
        auto r_12 = phys_c2.pos - phys_c1.pos;
        auto r_12_mag = sqrtf(r_12.x * r_12.x + r_12.y * r_12.y);
        if (r_12_mag > rend_c1.shape.getRadius() + rend_c2.shape.getRadius()) {
            continue;
        }

        // Calculate the momentum in the COM frame
        auto E = 0.5f * (phys_c1.mass * (phys_c1.vel.x * phys_c1.vel.x +
                                         phys_c1.vel.y * phys_c1.vel.y) +
                         phys_c2.mass * (phys_c2.vel.x * phys_c2.vel.x +
                                         phys_c2.vel.y * phys_c2.vel.y));
        auto T = (1. - collision_c.fraction_energy_lost) * E;
        auto pcom_mag = sqrtf(2.0f * T * phys_c1.mass * phys_c2.mass /
                              (phys_c1.mass + phys_c2.mass));
        auto p1com = -pcom_mag * r_12 / r_12_mag;

        // Convert back to default frame
        auto vcom = (phys_c1.vel * phys_c1.mass + phys_c2.vel * phys_c2.mass) /
                    (phys_c1.mass + phys_c2.mass);
        phys_c1.vel = vcom + p1com / phys_c1.mass;
        phys_c2.vel = vcom - p1com / phys_c2.mass;

        // Update durability
        auto U = collision_c.fraction_energy_converted * E;
        DurabilityComp* dur_c1 = registry.try_get<DurabilityComp>(entity1);
        if (dur_c1 != nullptr) {
            dur_c1->durability -= U * dur_c1->energy_to_durability;
        }
        DurabilityComp* dur_c2 = registry.try_get<DurabilityComp>(entity2);
        if (dur_c2 != nullptr) {
            dur_c2->durability -= U * dur_c2->energy_to_durability;
        }
    }
}

void PhysicsSystem::updateDurability(entt::registry& registry) {
    auto rview = registry.view<DurabilityComp, CircleComp, PhysicsComp>();
    for (auto [entity, dur_c, rend_c, phys_c] : rview.each()) {
        // Regenerate durability
        dur_c.durability += dur_c.durability_regen_rate * cfg.dt;

        // Cap durability at 1.0
        dur_c.durability_frac = dur_c.durability / dur_c.max_durability;
        if (dur_c.durability_frac > 1.0f) {
            dur_c.durability = dur_c.max_durability;
            rend_c.shape.setFillColor(sf::Color::White);
        } else if (dur_c.durability < 0.0f) {
            // When out of durability, set to 0 and change color
            dur_c.durability = 0.0f;
            if (dur_c.delete_at_zero) {
                registry.emplace<DeleteFlag>(entity);
            } else {
                rend_c.shape.setFillColor(sf::Color(63, 63, 63));
            }
        }
    }
}

}  // namespace twirl
