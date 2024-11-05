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
        auto r_mag = std::sqrt(r.x * r.x + r.y * r.y);
        auto r_hat = r / r_mag;

        auto r_mag_scaled = (r_mag + pairforce_comp.params.softening) / cfg.L;
        auto force = r_hat * pairforce_comp.params.magnitude * target_pc.mass *
                     source_pc.mass * powf(r_mag_scaled, pairforce_comp.params.power);

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

        // Reset force
        pc.force = {0.f, 0.f};
    }
}

RenderSystem::RenderSystem(const Config& cfg, sf::View& view) : cfg(cfg), view(view) {}

void RenderSystem::render(sf::RenderWindow& window, Components& components) {
    window.clear(sf::Color::Black);

    // draw frame
    for (auto [zorder, id] : components.entity_zorders) {
        auto& render_comp = components.render_comps.at(id);
        render_comp.shape.setPosition(components.physics_comps.at(id).pos);
        window.draw(render_comp.shape);
    }

    window.display();
}