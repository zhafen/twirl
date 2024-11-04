#include "systems.h"

#include <SFML/Graphics.hpp>

#include "game_objects.h"

RenderSystem::RenderSystem(const Config& cfg, sf::View& view) : cfg(cfg), view(view) {}

void RenderSystem::render(
    sf::RenderWindow& window,
    std::unordered_map<EntityId, RenderComponent>& render_components,
    std::unordered_map<EntityId, PhysicsComponent>& physics_components) {
    window.clear(sf::Color::Black);

    // draw frame
    for (auto& [id, render_comp] : render_components) {
        render_comp.shape.setPosition(physics_components.at(id).pos);
        window.draw(render_comp.shape);
    }

    window.display();
}