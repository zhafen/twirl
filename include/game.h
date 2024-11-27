#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "scene.h"
#include "system.h"


namespace twirl {

class Game {
   public:
    Game(std::string main_scene_fp);

    // Most of our methods are public
    void run();
    void resetGameState();
    void handleEvents();
    void update();
    void render();

    // Getter functions for private variables
    entt::registry& getRegistry() { return registry; }
    EntityMap& getEntityMap() { return entity_system.getEntityMap(registry); }
    sf::RenderWindow& getWindow() { return window; }
    sf::View& getView() { return view; }
    sf::View& getUiView() { return ui_view; }
    SceneSystem& getSceneSystem() { return scene_system; }
    EntitySystem& getEntitySystem() { return entity_system; }
    RenderSystem& getRenderSystem() { return render_system; }
    PhysicsSystem& getPhysicsSystem() { return physics_system; }
    std::string& getMainSceneFilePath() { return main_scene_fp; }

   private:
    std::string main_scene_fp;

    // ECS
    entt::registry registry;

    // Window and view
    sf::RenderWindow window;
    sf::View view;
    sf::View ui_view;

    // Systems
    SceneSystem scene_system;
    EntitySystem entity_system;
    RenderSystem render_system;
    PhysicsSystem physics_system;

    // Methods
    void initialize();
};

}  // namespace twirl

#endif  // GAME_H