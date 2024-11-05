#include "systems.h"

#include <SFML/Graphics.hpp>
#include <cmath>

#include "game_objects.h"

PhysicsSystem::PhysicsSystem(const Config& cfg) : cfg(cfg) {}

void PhysicsSystem::calculateForces(Components& components) {
    for (auto& [id, pairforce_comp] : components.pairforce_comps) {
        auto& target_pc = components.physics_comps.at(pairforce_comp.target_entity);
        auto& source_pc = components.physics_comps.at(pairforce_comp.source_entity);

        auto r = target_pc.pos - source_pc.pos;
        auto r_mag = std::sqrtf(r.x * r.x + r.y * r.y);
        auto r_hat = r / r_mag;

        auto r_mag_scaled = (r_mag + pairforce_comp.params.softening) / cfg.L;
        auto force = r_hat * pairforce_comp.params.magnitude * target_pc.mass *
                     source_pc.mass * powf(r_mag_scaled, pairforce_comp.params.power);

        target_pc.force += force;
    }
}

void PhysicsSystem::update(Components& components) {
    for (auto& [id, pc] : components.physics_comps) {
        // DEBUG: For some reason pc.force = 0
        // Update using leapfrog algorithm
        auto acc = pc.force / pc.mass;
        pc.vel += acc * cfg.dt / 2.f;
        pc.pos += pc.vel * cfg.dt;
        pc.vel += acc * cfg.dt / 2.f;

        // Update render component position
        auto render_comp = components.render_comps.at(id);
        render_comp.shape.setPosition(pc.pos);

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
    for (auto& [id1, pc1] : components.physics_comps) {
        auto rc1 = components.render_comps.at(id1);
        for (auto& [id2, pc2] : components.physics_comps) {
            if (id1 == id2) {
                continue;
            }
            auto rc2 = components.render_comps.at(id2);

            // Continue if not colliding
            if (!rc1.shape.getGlobalBounds().intersects(rc2.shape.getGlobalBounds())) {
                continue;
            }

            // Calculate the momentum in the COM frame
            auto T =
                0.5f * (pc1.mass * (pc1.vel.x * pc1.vel.x + pc1.vel.y * pc1.vel.y) +
                        pc2.mass * (pc2.vel.x * pc2.vel.x + pc2.vel.y * pc2.vel.y));
            auto pcom_mag =
                sqrtf(2.0f * T * pc1.mass * pc2.mass / (pc1.mass + pc2.mass));
            auto r_12 = pc2.pos - pc1.pos;
            auto r_12_mag = sqrtf(r_12.x * r_12.x + r_12.y * r_12.y);
            auto p1com = -pcom_mag * r_12 / r_12_mag;

            // Convert back to default frame
            auto vcom =
                (pc1.vel * pc1.mass + pc2.vel * pc2.mass) / (pc1.mass + pc2.mass);
            pc1.vel = vcom + p1com / pc1.mass;
            pc2.vel = vcom - p1com / pc2.mass;
        }
    }
}

RenderSystem::RenderSystem(const Config& cfg, sf::View& view) : cfg(cfg), view(view) {}

void RenderSystem::render(sf::RenderWindow& window, Components& components) {
    window.clear(sf::Color::Black);

    // draw frame
    for (auto [zorder, id] : components.entity_zorders) {
        auto& render_comp = components.render_comps.at(id);
        window.draw(render_comp.shape);
    }

    window.display();
}