#include <SFML/Graphics.hpp>
#include <cmath>

#include "system.h"

namespace cc {

GeneralSystem::GeneralSystem(const Config& cfg) : cfg(cfg) {}

void GeneralSystem::callPairwiseFunctions(Components& components) {
    for (auto& [rel_id, pfnc] : components.pairfunc_comps) {
        pfnc.func(pfnc.id1, pfnc.id2, components);
    }
}

PhysicsSystem::PhysicsSystem(const Config& cfg) : cfg(cfg) {}

void PhysicsSystem::calculateForces(Components& components) {
    for (auto& [id, fc] : components.force_comps) {
        auto& pc = components.physics_comps.at(id);
        float vel_mag = sqrtf(pc.vel.x * pc.vel.x + pc.vel.y * pc.vel.y);
        sf::Vector2f vel_scaling =
            (pc.vel / cfg.V) * powf(vel_mag / cfg.V, fc.drag_power - 1.0f);
        pc.force -= fc.drag_coefficient * cfg.A * vel_scaling;
    }
}

void PhysicsSystem::calculatePairwiseForces(Components& components) {
    for (auto& [rel_id, pfc] : components.pairforce_comps) {
        auto& target_pc = components.physics_comps.at(pfc.target_entity);
        auto& source_pc = components.physics_comps.at(pfc.source_entity);

        auto r = target_pc.pos - source_pc.pos;
        auto r_mag = sqrtf(r.x * r.x + r.y * r.y);

        // Don't calculate the distance when too close
        if (r_mag < pfc.params.min_distance * cfg.L) {
            continue;
        }

        auto r_hat = r / r_mag;
        auto r_mag_scaled = (r_mag + pfc.params.softening * cfg.L) / cfg.L /
                            pfc.params.distance_scaling;
        auto force = r_hat * pfc.params.magnitude * cfg.A * target_pc.mass *
                     source_pc.mass * powf(r_mag_scaled, pfc.params.power);

        target_pc.force += force;
    }
}

void PhysicsSystem::update(Components& components) {
    for (auto& [id, pc] : components.physics_comps) {
        // Update using leapfrog algorithm
        auto acc = pc.force / pc.mass;
        pc.vel += acc * cfg.dt / 2.f;
        pc.pos += pc.vel * cfg.dt;
        pc.vel += acc * cfg.dt / 2.f;

        // Update render component position
        auto& rc = components.render_comps.at(id);
        rc.shape->setPosition(pc.pos);

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
 * @param components A reference to the Components object containing physics and render
 * components.
 */
void PhysicsSystem::resolveCollisions(Components& components) {
    for (auto& [rel_id, cc] : components.collision_comps) {
        // Get first entity
        auto& id1 = cc.id1;
        auto& rc1 = components.render_comps.at(id1);
        auto& pc1 = components.physics_comps.at(id1);

        // Get second entity
        auto& id2 = cc.id2;
        auto& rc2 = components.render_comps.at(id2);
        auto& pc2 = components.physics_comps.at(id2);

        // Check for collision, assuming circular shapes for all objects that could
        // collide
        auto r_12 = pc2.pos - pc1.pos;
        auto r_12_mag = sqrtf(r_12.x * r_12.x + r_12.y * r_12.y);
        sf::CircleShape* rc1_shape = dynamic_cast<sf::CircleShape*>(rc1.shape.get());
        sf::CircleShape* rc2_shape = dynamic_cast<sf::CircleShape*>(rc2.shape.get());
        if (r_12_mag > rc1_shape->getRadius() + rc2_shape->getRadius()) {
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

void PhysicsSystem::updateDurability(Components& components) {
    for (auto& [id, dc] : components.dura_comps) {
        dc.durability += dc.durability_regen_rate * cfg.dt;

        auto& rc = components.render_comps.at(id);

        // Apply durability loss from collision
        bool& collided = components.physics_comps.at(id).collided;
        if (collided) {
            dc.durability -= dc.durability_loss_per_collision;
            collided = false;
        }

        // Cap durability at 1.0
        if (dc.durability > 1.0f) {
            dc.durability = 1.0f;
            rc.shape->setFillColor(sf::Color::White);
        } else if (dc.durability < 0.0f) {
            // When out of durability, set to 0 and change color
            dc.durability = 0.0f;
            rc.shape->setFillColor(sf::Color(63, 63, 63));
        } else {
            dc.durability += dc.durability_regen_rate * cfg.dt;
        }
    }
}

RenderSystem::RenderSystem(const Config& cfg, sf::View& view, sf::View& ui_view)
    : cfg(cfg), view(view), ui_view(ui_view) {}

void RenderSystem::render(EntityId player_id, sf::RenderWindow& window,
                          Components& components) {
    window.clear(sf::Color::Black);

    // draw frame
    for (auto [zorder, id] : components.entity_zorders) {
        auto& rc = components.render_comps.at(id);
        window.draw(*rc.shape);
    }
}

void RenderSystem::renderUI(sf::RenderWindow& window, Components& components) {
    window.setView(ui_view);

    // draw frame
    for (auto [zorder, id] : components.ui_entity_zorders) {
        auto& uic = components.ui_comps.at(id);

        // Get the bar out and set parameters
        sf::RectangleShape* bar = dynamic_cast<sf::RectangleShape*>(uic.shape.get());
        sf::Vector2f uic_size = uic.size;
        uic_size.x *= uic.tracked_value;
        // Causing the bar to shrink from the center requires changing both
        // the size and position
        sf::Vector2f uic_pos = uic.pos;
        uic_pos.x += (uic.size.x - uic_size.x) / 2.f;
        bar->setSize(uic_size);
        bar->setPosition(uic_pos);

        window.draw(*uic.shape);
    }
}

// EntitySystem::EntitySystem(const Config& cfg) : cfg(cfg) {}
// 
// // EntitySystem::spawnEntities(Components& components) {
// // }
// 
// void EntitySystem::removeEntity(Components& components, EntityId entity_id) {
//     components.physics_comps.erase(entity_id);
//     components.render_comps.erase(entity_id);
//     components.dura_comps.erase(entity_id);
// 
//     for (auto it = components.entity_zorders.begin(); it != components.entity_zorders.end(); ++it) {
//         if (it->second == entity_id) {
//             components.entity_zorders.erase(it);
//             break;
//         }
//     }
// } 

}  // namespace cc