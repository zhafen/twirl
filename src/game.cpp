#include "game.h"

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "game_objects.h"
#include "systems.h"

Game::Game()
    : cfg(),
      window(sf::VideoMode(cfg.window_size.x, cfg.window_size.y), "twirl"),
      view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size)),
      p(sf::Vector2f(0.f, 20.f * cfg.L), sf::Vector2f(0.f, -cfg.V), cfg.L, cfg),
      render_system() {
    window.setFramerateLimit(cfg.fps);
    initializeState();
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::resetGameState() {
    // Reset all systems
    // renderSystem.reset();

    // Reinitialize the game state
    initializeState();
}

int Game::createEntity() { return entityCounter++; }

void Game::initializeState() {
    // Create a RenderSystem instance and add it to the ECS
    auto renderSystem = std::make_unique<RenderSystem>();
    ecs.addSystem(std::move(renderSystem));

    // Create a new entity
    EntityID entityID = ecs.createEntity();

    // Create a RenderComponent and set its properties
    RenderComponent renderComponent;
    renderComponent.shape = sf::CircleShape(50);
    renderComponent.shape.setFillColor(sf::Color::Green);

    // Add the RenderComponent to the entity
    ecs.addComponent<RenderComponent>(entityID, renderComponent);

    // Add the entity to the RenderSystem
    RenderSystem* rs = dynamic_cast<RenderSystem*>(ecs.getComponent<System>(entityID));
    if (rs) {
        rs->addEntity(entityID, renderComponent);
    }

    // Make enemies
    std::random_device rd;
    std::mt19937 gen(rd());  // Standard random number generator
    std::uniform_real_distribution<float> dist(-10.f * cfg.L, 10.f * cfg.L);
    int n_enemies = 10;
    enemies.reserve(n_enemies);
    for (int i = 0; i < n_enemies; ++i) {
        enemies.emplace_back(sf::Vector2f(dist(gen), dist(gen)), sf::Vector2f(0.f, 0.f),
                             cfg.L, cfg);
        enemies[i].setFillColor(sf::Color::Red);
    }

    // Announcement text
    sf::Font font;
    if (!font.loadFromFile("../../Arial.ttf")) {
        std::cout << "No font found." << std::endl;
    }
    announcement.setFont(font);  // font is a sf::Font
    announcement.setString("Collision!");
    announcement.setCharacterSize(24);

    // Make some circles used for orientation
    int n_bkgrd = 100;
    for (int i = 0; i < n_bkgrd; ++i) {
        sf::CircleShape cir_i(10.f * cfg.L * (n_bkgrd - i));
        cir_i.setFillColor(sf::Color(127, 127, 127));
        cir_i.setOutlineThickness(cfg.L / 5.f);
        cir_i.setOutlineColor(sf::Color(63, 63, 63));
        cir_i.setOrigin(cir_i.getRadius(), cir_i.getRadius());
        bkgrd_circles.push_back(cir_i);
    }
}

void Game::handleEvents() {
    for (auto event = sf::Event(); window.pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void Game::update() {
    // Collision detection
    bool is_colliding = false;
    for (int i = 0; i < enemies.size(); ++i) {
        bool is_colliding_i =
            p.body_particle.getGlobalBounds().intersects(enemies[i].getGlobalBounds());
        is_colliding = is_colliding | is_colliding_i;
    }
    p.updateState(is_colliding);

    // Update game state
    for (int i = 0; i < enemies.size(); ++i) {
        sf::Vector2f r_et = p.body_particle.r - enemies[i].getPosition();
        float r2 = powf(r_et.x, 2.f) + powf(r_et.y, 2.f);
        sf::Vector2f a =
            5.f * cfg.A * (r_et / powf(r2 + powf(cfg.L, 2.f), 1.5f)) * cfg.L * cfg.L;
        enemies[i].updateState(a, cfg.dt);
    }

    if (p.health_bar.value <= 0.f) {
        resetGameState();
    }
}

void Game::render() {
    window.clear(sf::Color::Black);

    // Set the view
    view.setCenter(p.body_particle.r);
    window.setView(view);

    // draw frame
    for (int i = 0; i < bkgrd_circles.size(); ++i) {
        window.draw(bkgrd_circles[i]);
    }
    for (int i = 0; i < enemies.size(); ++i) {
        window.draw(enemies[i]);
    }
    p.draw(window, view);

    window.display();
}