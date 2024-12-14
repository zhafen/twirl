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
    void render(entt::registry& registry, sf::RenderWindow& window);
    void renderUI(entt::registry& registry, sf::RenderWindow& window);
    void setView(entt::registry& regsitry, sf::RenderWindow& window, sf::View& view);

    template <typename CompType>
    void renderShapeComp(entt::registry& registry, sf::RenderWindow& window) {
        // The .use ensures we use the order of render components
        auto rview = registry.view<ZOrderComp, CompType, PhysicsComp>();

        // draw frame
        for (auto [entity, zorder_c, shape_c, phys_c] : rview.each()) {
            if (std::isnan(phys_c.pos.x) || std::isnan(phys_c.pos.y)) {
                std::cerr << "Warning: Entity " << static_cast<int>(entity)
                          << " has NaN position.\n";
                continue;
            }

            shape_c.shape.setPosition(phys_c.pos);
            window.draw(shape_c.shape);
        }
    }

   private:
    sf::View view;
    sf::View ui_view;
};

}  // namespace twirl

#endif  // RENDER_H