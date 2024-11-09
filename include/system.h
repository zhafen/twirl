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
    void render(entt::entity player_id, sf::RenderWindow& window, entt::registry& registry);
    void renderUI(sf::RenderWindow& window, entt::registry& registry);

   private:
    sf::View view;
    sf::View ui_view;
    Config cfg;
};
// 
// class Scene {
//     public:
//      Scene(const Config& cfg);
//      void update(entt::registry& registry);
//      void render(sf::RenderWindow& window, entt::registry& registry);
//     
//     private:
//      Config cfg;
//      EntitySystem entity_system;
//      GeneralSystem general_system;
//      PhysicsSystem physics_system;
//      RenderSystem render_system;
// }

}  // namespace cc

#endif  // SYSTEMS_H