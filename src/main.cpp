#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

#include "game_objects.h"

int main() {
    Config cfg;

    // Window settings
    auto window = sf::RenderWindow({1920u, 1080u}, "twirl");
    window.setFramerateLimit(cfg.fps);

    // Create and activate a view
    sf::View view(sf::Vector2f(0, 0), sf::Vector2f(cfg.window_size));

    Player p(sf::Vector2f(0.f, 20.f * cfg.L), sf::Vector2f(0.f, -cfg.V), cfg.L, cfg);

    // Make enemies
    std::random_device rd;
    std::mt19937 gen(rd());  // Standard random number generator
    std::uniform_real_distribution<float> dist(-10.f * cfg.L, 10.f * cfg.L);
    std::vector<Particle> enemies;
    int n_enemies = 10;
    enemies.reserve(n_enemies);
    for (int i = 0; i < n_enemies; ++i) {
        enemies.emplace_back(sf::Vector2f(dist(gen), dist(gen)), sf::Vector2f(0.f, 0.f),
                         cfg.L, cfg);
        enemies[i].setFillColor(sf::Color::Red);
    }

    // Announcement text
    sf::Font font;
    if (!font.loadFromFile("./Arial.ttf")) {
        std::cout << "No font found." << std::endl;
    }
    sf::Text announcement;
    announcement.setFont(font);  // font is a sf::Font
    announcement.setString("Collision!");
    announcement.setCharacterSize(24);

    // Make some circles used for orientation
    std::vector<sf::CircleShape> bkgrd_circles(100);
    for (int i = 0; i < bkgrd_circles.size(); ++i) {
        sf::CircleShape cir_i(10.f * cfg.L * i);
        cir_i.setFillColor(sf::Color(127, 127, 127));
        cir_i.setOutlineThickness(cfg.L / 5.f);
        cir_i.setOutlineColor(sf::Color(63, 63, 63));
        cir_i.setOrigin(cir_i.getRadius(), cir_i.getRadius());
        bkgrd_circles[bkgrd_circles.size() - i - 1] = cir_i;
    }

    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Collision detection
        bool is_colliding = false;
        for (int i = 0; i < enemies.size(); ++i) {
            bool is_colliding_i = p.body_particle.getGlobalBounds().intersects(
                enemies[i].getGlobalBounds());
            is_colliding = is_colliding | is_colliding_i;
        }

        // Update game state
        for (int i = 0; i < enemies.size(); ++i) {
            sf::Vector2f r_et = p.body_particle.r - enemies[i].getPosition();
            float r2 = powf(r_et.x, 2.f) + powf(r_et.y, 2.f);
            sf::Vector2f a = 5.f * cfg.A * (r_et / powf(r2 + powf(cfg.L, 2.f), 1.5f)) *
                             cfg.L * cfg.L;
            enemies[i].updateState(a, cfg.dt);
        }
        p.updateState(is_colliding);

        // Set the view
        view.setCenter(p.body_particle.r);
        window.setView(view);

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw frame
        for (int i = 0; i < bkgrd_circles.size(); ++i) {
            window.draw(bkgrd_circles[i]);
        }
        for (int i = 0; i < enemies.size(); ++i) {
            window.draw(enemies[i]);
        }
        if (is_colliding) {
            window.draw(announcement);
        }
        p.draw(window, view);

        // end the current frame
        window.display();
    }
}
