#ifndef RENDER_H
#define RENDER_H

#include <functional>
#include <memory>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "components/component.h"
#include "config.h"
#include "shape.h"

namespace twirl {

class RenderSystem {
   public:
    RenderSystem(sf::View& view, sf::View& ui_view);
    void render(entt::registry& registry, sf::RenderWindow& window);
    void renderUI(entt::registry& registry, sf::RenderWindow& window);
    void setView(entt::registry& regsitry, sf::RenderWindow& window, sf::View& view);

   private:
    sf::View view;
    sf::View ui_view;
};

}   // namespace twirl

#endif // RENDER_H