#include "game.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>

// Position and Render components as before
struct PositionComponent {
    sf::Vector2f position;
};

struct RenderComponent {
    sf::RectangleShape shape;
};

// System base class
class System {
public:
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void reset() = 0;  // Added reset method
};

// Render system to manage Position and Render components
class RenderSystem : public System {
public:
    void addEntity(int entityID, const RenderComponent& renderComp, const PositionComponent& posComp) {
        renderComponents[entityID] = renderComp;
        positionComponents[entityID] = posComp;
    }

    void update() override {
        // Logic to update render components if needed
    }

    void render(sf::RenderWindow& window) override {
        for (auto& [id, renderComp] : renderComponents) {
            renderComp.shape.setPosition(positionComponents[id].position);
            window.draw(renderComp.shape);
        }
    }

    void reset() override {
        renderComponents.clear();
        positionComponents.clear();
    }

private:
    std::unordered_map<int, RenderComponent> renderComponents;
    std::unordered_map<int, PositionComponent> positionComponents;
};

class Game {
public:
    Game() : window(sf::VideoMode(800, 600), "ECS Pattern with Reset") {
        initializeState();
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

    void resetGameState() {
        // Reset all systems
        renderSystem.reset();

        // Reinitialize the game state
        initializeState();
    }

private:
    sf::RenderWindow window;
    RenderSystem renderSystem;
    int entityCounter = 0;

    int createEntity() { return entityCounter++; }

    void initializeState() {
        // Create initial game entities
        int playerID = createEntity();
        PositionComponent playerPos = { {100, 100} };
        RenderComponent playerRender;
        playerRender.shape.setSize(sf::Vector2f(50, 50));
        playerRender.shape.setFillColor(sf::Color::Green);

        // Add components to the render system
        renderSystem.addEntity(playerID, playerRender, playerPos);

        // Additional initialization for other entities can go here
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                resetGameState();  // Reset game state on pressing "R"
            }
        }
    }

    void update() {
        renderSystem.update();
    }

    void render() {
        window.clear();
        renderSystem.render(window);
        window.display();
    }
};