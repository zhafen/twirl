#ifndef RENDER_H
#define RENDER_H

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

#include "components/components.h"
#include "config.h"
#include "shape.h"

namespace twirl {

class RenderSystem {
   public:
    RenderSystem(sf::View& view, sf::View& ui_view);
    void prepareRender(entt::registry& registry, sf::RenderWindow& window);
    void render(entt::registry& registry, sf::RenderWindow& window);
    void renderUI(entt::registry& registry, sf::RenderWindow& window);
    void setView(entt::registry& regsitry, sf::RenderWindow& window, sf::View& view);

    template <typename CompType>
    void syncRenderAndPhysics(entt::registry& registry) {
        auto rview = registry.view<CompType, PhysicsComp>();

        for (auto [entity, comp, phys_c] : rview.each()) {
            if (std::isnan(phys_c.pos.x) || std::isnan(phys_c.pos.y)) {
                std::cerr << "Warning: Entity " << static_cast<int>(entity)
                          << " has NaN position.\n";
                continue;
            }
            comp.shape.setPosition(phys_c.pos);
        }
    }

    template <typename CompType, typename FlagType>
    void renderShapeComp(entt::registry& registry, sf::RenderWindow& window) {
        auto rview = registry.view<ZOrderComp, FlagType, CompType, ColorComp>();

        // draw frame
        for (auto [entity, zorder_c, shape_c, color_c] : rview.each()) {
            shape_c.shape.setFillColor(color_c.fill_color);
            shape_c.shape.setOutlineColor(color_c.outline_color);
            window.draw(shape_c.shape);
        }
    }

   private:
    sf::View view;
    sf::View ui_view;
};

}  // namespace twirl

#endif  // RENDER_H