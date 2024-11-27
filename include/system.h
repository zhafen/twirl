#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

#include "component.h"
#include "config.h"

namespace twirl {

class EntitySystem {
   public:
    EntitySystem() = default;
    EntityMap getEntityMap(entt::registry& registry);
    entt::entity resolveEntityName(entt::registry& registry, EntityName name,
                                   entt::entity entity);
    void resolveEntityPairs(entt::registry& registry);
    void spawnEntities(entt::registry& registry);
    void deleteEntities(entt::registry& registry);
    void syncEntities(entt::registry& registry);
    void orderEntities(entt::registry& registry);

   private:
    EntityMap entity_map;
    bool needs_ordering = true;
};

class PhysicsSystem {
   public:
    PhysicsSystem() = default;
    void calculateForces(entt::registry& registry);
    void calculatePairwiseForces(entt::registry& registry);
    void update(entt::registry& registry);
    void updateStopWatches(entt::registry& registry);
    void resolveCollisions(entt::registry& registry);
    void updateDurability(entt::registry& registry);
};

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

}  // namespace twirl

#endif  // SYSTEMS_H