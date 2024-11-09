#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

#include "config.h"

namespace cc {

// Probably best to avoid using the general system when possible
class GeneralSystem {
   public:
    GeneralSystem(const Config& cfg);
    void callPairwiseFunctions(Components& components);

   private:
    Config cfg;
};

class EntitySystem {
   public:
    EntitySystem(const Config& cfg);
    EntityId spawnEntity(Components& components);
    void removeEntity(Components& components, EntityId entity_id);

   private:
    Config cfg;
};

class PhysicsSystem {
   public:
    PhysicsSystem(const Config& cfg);
    void calculateForces(Components& components);
    void calculatePairwiseForces(Components& components);
    void update(Components& components);
    void resolveCollisions(Components& components);
    void updateDurability(Components& components);

   private:
    Config cfg;
};

class RenderSystem {
   public:
    RenderSystem(const Config& cfg, sf::View& view, sf::View& ui_view);
    void render(EntityId player_id, sf::RenderWindow& window, Components& components);
    void renderUI(sf::RenderWindow& window, Components& components);

   private:
    sf::View view;
    sf::View ui_view;
    Config cfg;
};
// 
// class Scene {
//     public:
//      Scene(const Config& cfg);
//      void update(Components& components);
//      void render(sf::RenderWindow& window, Components& components);
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