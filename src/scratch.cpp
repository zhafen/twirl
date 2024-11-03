#include <SFML/Graphics.hpp>

// Position and Render components as before
struct PhysicsComponent {
    sf::Vector2f position;
    sf::Vector2f velocity;
};

struct RenderComponent {
    sf::RectangleShape shape;
};

// System base class
class System {
public:
    virtual void update() = 0;
    virtual void reset() = 0;  // Added reset method
};

// Render system to manage Position and Render components
class RenderSystem : public System {
public:
    void addEntity(int entityID, const RenderComponent& renderComp, const PhysicsComponent& posComp) {
        renderComponents[entityID] = renderComp;
        physicsComponents[entityID] = posComp;
    }

    void update() override {
        // Logic to update render components if needed
    }

    void render(sf::RenderWindow& window) {
        for (auto& [id, renderComp] : renderComponents) {
            renderComp.shape.setPosition(physicsComponents[id].position);
            window.draw(renderComp.shape);
        }
    }

    void reset() override {
        renderComponents.clear();
        physicsComponents.clear();
    }

private:
    std::unordered_map<int, RenderComponent> renderComponents;
    std::unordered_map<int, PhysicsComponent> physicsComponents;
};

class PhysicsSystem : public System {
public:
    void addEntity(int entityID, const PhysicsComponent& physicsComp) {
        physicsComponents[entityID] = physicsComp;
    }

    void update() override {
        for (auto& [id, physicsComp] : physicsComponents) {
            physicsComp.position += physicsComp.velocity;
        }
    }

    void reset() override {
        physicsComponents.clear();
    }

private:
    std::unordered_map<int, PhysicsComponent> physicsComponents;
};

void Game::initializeState() {
    // Create initial game entities
    int playerID = createEntity();
    PhysicsComponent playerPos = { {100, 100} };
    RenderComponent playerRender;
    playerRender.shape.setSize(sf::Vector2f(50, 50));
    playerRender.shape.setFillColor(sf::Color::Green);

    // Add components to the render system
    renderSystem.addEntity(playerID, playerRender, playerPos);

    // Additional initialization for other entities can go here
}