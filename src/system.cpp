#include "system.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <entt/entt.hpp>

namespace cc {

EntitySystem::EntitySystem(const Config& cfg) : cfg(cfg) {}

void EntitySystem::spawnEntities(entt::registry& registry) {

    // Create a new projectile
    auto projectile = registry.create();
    registry.emplace<PhysicsComp>(projectile);
    registry.emplace<DragForceComp>(projectile);
    registry.emplace<DurabilityComp>(projectile);
    auto& rc = registry.emplace<RenderComp>(projectile, CCCircleShape(cfg.L / 20.f));
    rc.shape.setFillColor(sf::Color::Blue);

    auto rview = registry.view<EnemyComp>();
    for (auto [enemy, ec] : rview.each()) {
        // Target the enemies
        auto relation = registry.create();
        auto& pfc = registry.emplace<PairwiseForceComp>(relation, projectile, enemy);
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

PhysicsSystem::PhysicsSystem(const Config& cfg) : cfg(cfg) {}

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
    auto rview = registry.view<PairwiseForceComp>();

    for (auto [entity, pfc] : rview.each()) {
        auto& target_pc = registry.get<PhysicsComp>(pfc.target_entity);
        auto& source_pc = registry.get<PhysicsComp>(pfc.source_entity);

        auto r = target_pc.pos - source_pc.pos;
        auto r_mag = sqrtf(r.x * r.x + r.y * r.y);

        // Don't calculate the distance when too close
        if (r_mag < pfc.params.min_distance * cfg.L) {
            continue;
        }

        auto r_hat = r / r_mag;
        auto r_mag_scaled =
            (r_mag / cfg.L + pfc.params.softening) / pfc.params.distance_scaling;
        auto force = cfg.A * r_hat * pfc.params.magnitude * target_pc.mass *
                     source_pc.mass * powf(r_mag_scaled, pfc.params.power);

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
 *
 * @param components A reference to the Comps object containing physics and render
 * components.
 */
void PhysicsSystem::resolveCollisions(entt::registry& registry) {
    for (auto [rel_id, cc] : registry.view<CollisionComp>().each()) {
        // Get first entity
        auto& entity1 = cc.entity1;
        auto& pc1 = registry.get<PhysicsComp>(entity1);
        auto& rc1 = registry.get<RenderComp>(entity1);

        // Get second entity
        auto& entity2 = cc.entity2;
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
            rc.shape.setFillColor(sf::Color(63, 63, 63));
        }
    }
}

RenderSystem::RenderSystem(const Config& cfg, sf::View& view, sf::View& ui_view)
    : cfg(cfg), view(view), ui_view(ui_view) {}

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

void RenderSystem::renderUI(sf::RenderWindow& window, entt::registry& registry) {
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

// EntitySystem::EntitySystem(const Config& cfg) : cfg(cfg) {}
//
// // EntitySystem::spawnEntities(entt::registry& registry) {
// // }
//
// void EntitySystem::removeEntity(entt::registry& registry, EntityId entity_id) {
//     components.physics_comps.erase(entity_id);
//     components.render_comps.erase(entity_id);
//     components.dura_comps.erase(entity_id);
//
//     for (auto it = components.rc_zorders.begin(); it != components.rc_zorders.end();
//     ++it) {
//         if (it->second == entity_id) {
//             components.rc_zorders.erase(it);
//             break;
//         }
//     }
// }

}  // namespace cc