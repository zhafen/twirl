#include "systems/render.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <iostream>

#include "components/component.h"
#include "config.h"
#include "shape.h"

namespace twirl {

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