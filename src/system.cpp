#include "system.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <iostream>

#include "config.h"

namespace twirl {

EntityMap EntitySystem::getEntityMap(entt::registry& registry) {
    EntityMap entity_map;

    auto rview = registry.view<EntityName>();
    for (auto [entity, name] : rview.each()) {
        entity_map[name] = entity;
    }

    return entity_map;
}

/**
 * OPTIMIZE: This function may not be able to loop over every paircomp if we use
 * listeners, and should probably not call getEntityMap every time.
 */
void EntitySystem::resolveEntityPairs(entt::registry& registry) {
    auto entity_map = getEntityMap(registry);
    auto rview = registry.view<PairComp>();
    for (auto [pair_entity, pc] : rview.each()) {
        bool destroy_pair_entity = false;
        if (!registry.valid(pc.target_entity)) {
            // If the the target name is not empty, try to resolve it
            if (!pc.target_entity_name.empty()) {
                // Try to resolve and use the target name
                bool is_resolved =
                    entity_map.find(pc.target_entity_name) != entity_map.end();
                if (is_resolved) {
                    pc.target_entity = entity_map[pc.target_entity_name];
                } else {
                    // If the name cannot be resolved,
                    // mark the pair entity for destruction
                    destroy_pair_entity = true;
                }
            } else {
                // If not valid and there's no name, mark for destruction
                destroy_pair_entity = true;
            }
        }
        if (!registry.valid(pc.source_entity)) {
            // If the the target name is not empty, try to resolve it
            if (!pc.source_entity_name.empty()) {
                // Try to resolve and use the target name
                bool is_resolved =
                    entity_map.find(pc.source_entity_name) != entity_map.end();
                if (is_resolved) {
                    pc.source_entity = entity_map[pc.source_entity_name];
                } else {
                    // If the name cannot be resolved,
                    // mark the pair entity for destruction
                    destroy_pair_entity = true;
                }
            } else {
                // If not valid and there's no name, mark for destruction
                destroy_pair_entity = true;
            }

        // TODO: Delete resolved pair comps
        }
        // Follow through, destroying only once
        if (destroy_pair_entity) {
            registry.destroy(pair_entity);
        }
    }
}

void EntitySystem::spawnEntities(entt::registry& registry) {
    auto rview = registry.view<SceneTriggerComp, StopWatchComp>();
    for (auto [entity, asc, swc] : rview.each()) {
        // Check if the end time was reached
        if (!swc.end_reached) {
            continue;
        } else {
            // Reset the stopwatch
            swc.current_time = 0.0f;
            swc.end_reached = false;
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

void PhysicsSystem::calculateForces(entt::registry& registry) {
    auto rview = registry.view<PhysicsComp, DragForceComp>();
    for (auto [entity, pc, fc] : rview.each()) {
        float vel_mag = sqrtf(pc.vel.x * pc.vel.x + pc.vel.y * pc.vel.y);
        sf::Vector2f vel_scaling =
            (pc.vel / cfg.V) * powf(vel_mag / cfg.V, fc.drag_power - 1.0f);
        pc.force -= cfg.A * fc.drag_coefficient * vel_scaling;
    }
}

void PhysicsSystem::calculatePairwiseForces(entt::registry& registry) {
    auto rview = registry.view<PairComp, PairwiseForceComp>();

    for (auto [rel_id, prc, pfc] : rview.each()) {
        auto& target_pc = registry.get<PhysicsComp>(prc.target_entity);
        auto& source_pc = registry.get<PhysicsComp>(prc.source_entity);

        auto r = target_pc.pos - source_pc.pos;
        auto r_mag = sqrtf(r.x * r.x + r.y * r.y);

        // Don't calculate the distance when too close
        if (r_mag < pfc.min_distance * cfg.L) {
            continue;
        }

        auto r_hat = r / r_mag;
        auto r_mag_scaled = (r_mag / cfg.L + pfc.softening) / pfc.distance_scaling;
        auto force = cfg.A * r_hat * pfc.magnitude * target_pc.mass * source_pc.mass *
                     powf(r_mag_scaled, pfc.power);

        target_pc.force += force;
    }
}

void PhysicsSystem::update(entt::registry& registry) {
    auto rview = registry.view<PhysicsComp>();

    for (auto [entity, pc] : rview.each()) {
        // Update using leapfrog algorithm
        auto acc = pc.force / pc.mass;
        pc.vel += acc * cfg.dt / 2.f;
        pc.pos += pc.vel * cfg.dt;
        pc.vel += acc * cfg.dt / 2.f;

        // Reset force
        pc.force = {0.f, 0.f};
    }
}

void PhysicsSystem::updateStopWatches(entt::registry& registry) {
    auto rview = registry.view<StopWatchComp>();
    for (auto [entity, swc] : rview.each()) {
        // Keep going if the end time was already reached.
        if (swc.end_reached) {
            continue;
        }

        swc.current_time += cfg.dt;

        if (swc.current_time >= swc.end_time) {
            swc.end_reached = true;
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
    for (auto [rel_id, prc] : registry.view<PairComp, CollisionComp>().each()) {
        // Check if the entities still exist
        if (!registry.valid(prc.target_entity) || !registry.valid(prc.source_entity)) {
            registry.destroy(rel_id);
            continue;
        }

        // Get first entity
        auto& entity1 = prc.target_entity;
        auto& pc1 = registry.get<PhysicsComp>(entity1);
        auto& rc1 = registry.get<RenderComp>(entity1);

        // Get second entity
        auto& entity2 = prc.source_entity;
        auto& pc2 = registry.get<PhysicsComp>(entity2);
        auto& rc2 = registry.get<RenderComp>(entity2);

        // Check for collision, assuming circular shapes for all objects that collide
        auto r_12 = pc2.pos - pc1.pos;
        auto r_12_mag = sqrtf(r_12.x * r_12.x + r_12.y * r_12.y);
        if (r_12_mag > rc1.shape.getRadius() + rc2.shape.getRadius()) {
            continue;
        }

        // Calculate the momentum in the COM frame
        auto T = 0.5f * (pc1.mass * (pc1.vel.x * pc1.vel.x + pc1.vel.y * pc1.vel.y) +
                         pc2.mass * (pc2.vel.x * pc2.vel.x + pc2.vel.y * pc2.vel.y));
        auto pcom_mag = sqrtf(2.0f * T * pc1.mass * pc2.mass / (pc1.mass + pc2.mass));
        auto p1com = -pcom_mag * r_12 / r_12_mag;

        // Convert back to default frame
        auto vcom = (pc1.vel * pc1.mass + pc2.vel * pc2.mass) / (pc1.mass + pc2.mass);
        pc1.vel = vcom + p1com / pc1.mass;
        pc2.vel = vcom - p1com / pc2.mass;

        // Indicate collision
        pc1.collided = true;
        pc2.collided = true;
    }
}

void PhysicsSystem::updateDurability(entt::registry& registry) {
    auto rview = registry.view<DurabilityComp, RenderComp, PhysicsComp>();
    for (auto [entity, dc, rc, pc] : rview.each()) {
        // Regenerate durability
        dc.durability += dc.durability_regen_rate * cfg.dt;

        // Apply durability loss from collision
        if (pc.collided) {
            dc.durability -= dc.durability_loss_per_collision;
            pc.collided = false;
        }

        // Cap durability at 1.0
        if (dc.durability > 1.0f) {
            dc.durability = 1.0f;
            rc.shape.setFillColor(sf::Color::White);
        } else if (dc.durability < 0.0f) {
            // When out of durability, set to 0 and change color
            dc.durability = 0.0f;
            if (dc.delete_at_zero) {
                registry.emplace<DeleteComp>(entity);
            } else {
                rc.shape.setFillColor(sf::Color(63, 63, 63));
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
    for (auto [entity, rc, pc] : rview.each()) {
        rc.shape.setPosition(pc.pos);
        window.draw(rc.shape);
    }
}

void RenderSystem::renderUI(entt::registry& registry, sf::RenderWindow& window) {
    window.setView(ui_view);

    // draw frame
    for (auto [entity, uic] : registry.view<UIComp>().each()) {
        // Get versions of the size and position that can be modified
        auto uic_size = uic.size;
        auto uic_pos = uic.pos;

        // Scale size with the tracked value
        uic_size.x *= *uic.tracked_value;
        // Causing the bar to shrink from the center requires changing both
        // the size and position
        uic_pos.x += (uic.size.x - uic_size.x) / 2.f;
        uic.shape.setSize(uic_size);
        uic.shape.setPosition(uic_pos);

        window.draw(uic.shape);
    }
}

void RenderSystem::setView(entt::registry& registry, sf::RenderWindow& window,
                           sf::View& view) {
    // Pin the view to the ViewComp entity (the player)
    auto rview = registry.view<PhysicsComp, ViewComp>();
    size_t n_vc = 0;
    for (auto [entity, pc] : rview.each()) {
        if (n_vc > 1) {
            std::cerr << "Warning: More than one ViewComp entity found. Using the "
                         "first one.\n";
            break;
        }
        view.setCenter(pc.pos);
        window.setView(view);
        n_vc++;
    }
}

}  // namespace twirl