#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include "particle.h"

int main() {
    // Window settings
    auto window = sf::RenderWindow({1920u, 1080u}, "twirl");
    int fps = 144;
    window.setFramerateLimit(fps);
    // By setting dt to not vary depending on how long it took to generate a frame,
    // our game slows down when it takes longer to render.
    float dt = 1.f / fps;
    sf::Vector2u window_size = window.getSize();
    // d will be our length scale, and we set it to a multiple of window size
    // We also introduce velocity, time, and acceleration scales
    float d(0.01f * window_size.x);
    float t(0.1f);
    float s(d / t);
    float g(s / t);
    float dx(s * dt);

    // Create and activate a view
    sf::View view(sf::Vector2f(0, 0), sf::Vector2f(window_size.x, window_size.y));

    Particle p(sf::Vector2f(0.f, 20.f * d), sf::Vector2f(0.f, -s), d);

    Particle target(p.r, p.v, d / 2.f);
    target.setFillColor(sf::Color::Black);
    target.setOutlineThickness(d / 10.f);
    target.setOutlineColor(sf::Color::White);
    sf::Vector2f r_pf(sf::Vector2f(0.f, 0.f));

    // Make enemies
    std::random_device rd;
    std::mt19937 gen(rd());  // Standard random number generator
    std::uniform_real_distribution<float> dist(-10.f * d, 10.f * d);
    std::vector<sf::CircleShape> enemy_circles(10);
    for (int i = 0; i < enemy_circles.size(); ++i) {
        sf::CircleShape cir_i(d);
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
    announcement.setFont(font); // font is a sf::Font
    announcement.setString("Collision!");
    announcement.setCharacterSize(24);

    // Health bar
    float health_bar_size = window_size.x / 2.f;
    sf::RectangleShape health_bar(sf::Vector2f(health_bar_size, d));
    health_bar.setFillColor(sf::Color::White);
    health_bar.setOutlineThickness(d / 10.f);
    health_bar.setOutlineColor(sf::Color::Black);
    health_bar.setOrigin(health_bar_size / 2.f, 0.f);

    // Make some circles used for orientation
    std::vector<sf::CircleShape> bkgrd_circles(100);
    for (int i = 0; i < bkgrd_circles.size(); ++i) {
        sf::CircleShape cir_i(10.f * d * i);
        cir_i.setFillColor(sf::Color(127, 127, 127));
        cir_i.setOutlineThickness(d / 5.f);
        cir_i.setOutlineColor(sf::Color(63, 63, 63));
        cir_i.setOrigin(cir_i.getRadius(), cir_i.getRadius());
        bkgrd_circles[bkgrd_circles.size() - i - 1] = cir_i;
    }

    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                // get the current mouse position in the window
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                // convert it to world coordinates
                target.r = window.mapPixelToCoords(pixelPos);
                target.setPosition(target.r);
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) {
            r_pf += sf::Vector2f(dx, 0.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left)) {
            r_pf += sf::Vector2f(-dx, 0.f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up)) {
            r_pf += sf::Vector2f(0.f, -dx);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down)) {
            r_pf += sf::Vector2f(0.f, dx);
        }
        target.setPosition(p.r + r_pf);

        // Update health bar position to always be centered in view
        sf::Vector2f healthBarPosition(window_size.x / 2.f, d);
        health_bar.setPosition(window.mapPixelToCoords(sf::Vector2i(healthBarPosition)));

        // Gravitational force
        sf::Vector2f r = target.r - p.r;
        float r2 = pow(r.x, 2.f) + pow(r.y, 2.f);
        sf::Vector2f a = 5.f * g * (r / pow(r2 + pow(d, 2.f), 1.5f)) * d * d;

        // Collision detection
        bool any_collision = false;
        for (int i = 0; i < enemy_circles.size(); ++i) {
            bool is_colliding = p.getGlobalBounds().intersects(
                enemy_circles[i].getGlobalBounds());
            any_collision = any_collision | is_colliding;
        }
        announcement.setPosition(window.mapPixelToCoords(sf::Vector2i(window_size.x / 2, 0)));


        // clear the window with black color
        window.clear(sf::Color::Black);
        p.applyPhysics(a, dt);

        // draw frame
        for (int i = 0; i < bkgrd_circles.size(); ++i) {
            window.draw(bkgrd_circles[i]);
        }
        for (int i = 0; i < enemy_circles.size(); ++i) {
            window.draw(enemy_circles[i]);
        }
        window.draw(p);
        window.draw(target);
        if (any_collision) {
            window.draw(announcement);
        }
        window.draw(health_bar);

        // Set the view
        view.setCenter(p.r);
        window.setView(view);

        // end the current frame
        window.display();
    }
}
