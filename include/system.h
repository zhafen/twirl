#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <functional>
#include <memory>
#include <unordered_map>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include "config.h"
#include "component.h"

namespace cc {

// Probably best to avoid using the general system when possible
class GeneralSystem {
   public:
    GeneralSystem(const Config& cfg);
    void callPairwiseFunctions(entt::registry& registry);

   private:
    Config cfg;
};

class EntitySystem {
   public:
    EntitySystem(const Config& cfg);
    void spawnEntities(entt::registry& registry);
    void orderEntities(entt::registry& registry);

   private:
    Zorders zorders;
    bool needs_ordering = true;
    Config cfg;
};

class PhysicsSystem {
   public:
    PhysicsSystem(const Config& cfg);
    void calculateForces(entt::registry& registry);
    void calculatePairwiseForces(entt::registry& registry);
    void update(entt::registry& registry);
    void resolveCollisions(entt::registry& registry);
    void updateDurability(entt::registry& registry);

   private:
    Config cfg;
};

class RenderSystem {
   public:
    RenderSystem(const Config& cfg, sf::View& view, sf::View& ui_view);
    void render(entt::registry& registry, sf::RenderWindow& window);
    void renderUI(sf::RenderWindow& window, entt::registry& registry);

   private:
    sf::View view;
    sf::View ui_view;
    Config cfg;
};

}  // namespace cc

#endif  // SYSTEMS_H