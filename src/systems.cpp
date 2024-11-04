#include "systems.h"

#include <SFML/Graphics.hpp>

#include "game_objects.h"

RenderSystem::RenderSystem(const Config& cfg, sf::View& view) : cfg(cfg), view(view) {}

void RenderSystem::render(
    sf::RenderWindow& window,
    const std::unordered_map<EntityId, RenderComponent>& render_components) {
    window.clear(sf::Color::Black);

    // draw frame
    for (auto& [id, render_comp] : render_components) {
        window.draw(render_comp.shape);
    }

    window.display();
}