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
    std::vector<sf::CircleShape> enemy_circles(10);
    for (int i = 0; i < enemy_circles.size(); ++i) {
        sf::CircleShape cir_i(cfg.L);
        cir_i.setFillColor(sf::Color::Red);
        cir_i.setPosition(dist(gen), dist(gen));
        enemy_circles[i] = cir_i;
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

            // if (event.type == sf::Event::MouseButtonPressed) {
            //     // get the current mouse position in the window
            //     sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            //     // convert it to world coordinates
            //     target.r = window.mapPixelToCoords(pixelPos);
            //     target.setPosition(target.r);
            // }
        }

        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) {
        //     r_pf += sf::Vector2f(cfg.dx, 0.f);
        // }
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left)) {
        //     r_pf += sf::Vector2f(-cfg.dx, 0.f);
        // }
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) {
        //     r_pf += sf::Vector2f(0.f, -cfg.dx);
        // }
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down)) {
        //     r_pf += sf::Vector2f(0.f, cfg.dx);
        // }

        // // Gravitational force
        // sf::Vector2f r = target.r - p.r;
        // float r2 = powf(r.x, 2.f) + powf(r.y, 2.f);
        // sf::Vector2f a =
        //     5.f * cfg.A * (r / powf(r2 + powf(cfg.L, 2.f), 1.5f)) * cfg.L * cfg.L;

        // Collision detection
        bool any_collision = false;
        for (int i = 0; i < enemy_circles.size(); ++i) {
            bool is_colliding =
                p.body.getGlobalBounds().intersects(enemy_circles[i].getGlobalBounds());
            any_collision = any_collision | is_colliding;
        }

        // clear the window with black color
        window.clear(sf::Color::Black);
        // p.body.updateState(a, cfg.dt);

        // draw frame
        for (int i = 0; i < bkgrd_circles.size(); ++i) {
            window.draw(bkgrd_circles[i]);
        }
        for (int i = 0; i < enemy_circles.size(); ++i) {
            window.draw(enemy_circles[i]);
        }
        if (any_collision) {
            window.draw(announcement);
        }
        p.draw(window);

        // Set the view
        view.setCenter(p.body.r);
        window.setView(view);

        // end the current frame
        window.display();
    }
}
