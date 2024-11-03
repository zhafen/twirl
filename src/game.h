#include <SFML/Graphics.hpp>

#include "game_objects.h"

class Game {
    public:
     Game();
     void run();
     void resetGameState();

    private:
     // Configuration
     Config cfg;

     // Window and view
     sf::RenderWindow window;
     sf::View view;

     // Entities
     int entityCounter = 0;
     Player p;
     std::vector<Particle> enemies;
     std::vector<sf::CircleShape> bkgrd_circles;

     // UI Elements
     sf::Text announcement;

     // Systems
     // RenderSystem renderSystem;

     // Methods
     int createEntity();
     void initializeState();
     void handleEvents();
     void update();
     void render();
};;