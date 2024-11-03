#include <SFML/Graphics.hpp>
class Game {
   public:
    Game();

    void run();

    void resetGameState();

   private:
    // Entities
    int entityCounter = 0;
    Player p;
    std::vector<Particle> enemies;
    sf::Text announcement;
    std::vector<sf::CircleShape> bkgrd_circles;

    Config cfg;
    sf::RenderWindow window;
    sf::View view;

    // RenderSystem renderSystem;

    int createEntity();

    void initializeState();

    void handleEvents();

    void update();

    void render();
};