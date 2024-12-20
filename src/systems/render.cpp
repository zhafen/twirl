#include "systems/render.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <iostream>

#include "components/components.h"
#include "config.h"
#include "shape.h"

namespace twirl {

RenderSystem::RenderSystem(sf::View& view, sf::View& ui_view)
    : view(view), ui_view(ui_view) {}

void RenderSystem::prepareRender(entt::registry& registry, sf::RenderWindow& window) {

    // This ensures that the render components are in the correct position
    // when they align with a physics component
    syncRenderAndPhysics<CircleComp>(registry);
    syncRenderAndPhysics<RectangleComp>(registry);

    window.clear(sf::Color::Black);
}

void RenderSystem::render(entt::registry& registry, sf::RenderWindow& window) {
    renderShapeComp<CircleComp, RenderFlag>(registry, window);
    renderShapeComp<RectangleComp, RenderFlag>(registry, window);
}

void RenderSystem::renderUI(entt::registry& registry, sf::RenderWindow& window) {
    window.setView(ui_view);

    renderShapeComp<CircleComp, UIRenderFlag>(registry, window);
    renderShapeComp<RectangleComp, UIRenderFlag>(registry, window);

    // Draw value bars
    for (auto [entity, vb_c] : registry.view<ValueBarComp>().each()) {
        // Get versions of the size and position that can be modified
        auto vb_c_size = vb_c.size;
        auto vb_c_pos = vb_c.pos;

        // Scale size with the tracked value
        vb_c_size.x *= *vb_c.tracked_value;
        // Causing the bar to shrink from the center requires changing both
        // the size and position
        vb_c_pos.x += (vb_c.size.x - vb_c_size.x) / 2.f;
        vb_c.shape.setSize(vb_c_size);
        vb_c.shape.setPosition(vb_c_pos);

        window.draw(vb_c.shape);
    }

    // Draw text
    for (auto [entity, text_c] : registry.view<TextComp>().each()) {
        window.draw(text_c.text);
    }
}

void RenderSystem::setView(entt::registry& registry, sf::RenderWindow& window,
                           sf::View& view) {
    // Pin the view to the ViewFlag entity (the player)
    auto rview = registry.view<PhysicsComp, ViewFlag>();
    size_t n_vc = 0;
    for (auto [entity, phys_c] : rview.each()) {
        if (n_vc > 1) {
            std::cerr << "Warning: More than one ViewFlag entity found. Using the "
                         "first one.\n";
            break;
        }
        view.setCenter(phys_c.pos);
        window.setView(view);
        n_vc++;
    }
}

}  // namespace twirl