#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

class Particle {
   public:
    Particle(sf::Vector2f r, sf::Vector2f v, float R) : r(r), v(v), shape(R) {
        // Set the origin as the center of the particle
        shape.setOrigin(R, R);
        shape.setPosition(r);
    }

    void update(sf::Vector2f a, float dt) {
        // Update using leapfrog algorithm
        v += a * dt / 2.f;
        r += v * dt;
        v += a * dt / 2.f;
        shape.setPosition(r);
    }
    sf::Vector2f r;
    sf::Vector2f v;
    sf::CircleShape shape;

    sf::CircleShape getShape() const { return shape; }
};

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

    // Create and activate a view
    sf::View view(sf::Vector2f(0, 0), sf::Vector2f(window_size.x, window_size.y));

    Particle p(sf::Vector2f(0.f, 20.f * d), sf::Vector2f(0.f, -s), d);

    Particle p2(sf::Vector2f(5.f * d, 5.f * d), sf::Vector2f(0.f, 0.f), d);

    sf::RectangleShape rectangle(sf::Vector2f(10.f * d, 10.f * d));
    rectangle.setPosition(0.f, 0.f);
    rectangle.setFillColor(sf::Color(128, 128, 128));

    // set the mouse position locally (relative to a window)
    sf::Mouse::setPosition(sf::Vector2i(0.f, 0.f), window);

    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                // get the current mouse position in the window
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                // convert it to world coordinates
                p2.r = window.mapPixelToCoords(pixelPos);
                p2.shape.setPosition(p2.r);
            }
        }

        // Gravitational force
        sf::Vector2f r = p2.r - p.r;
        float r2 = pow(r.x, 2.f) + pow(r.y, 2.f);
        sf::Vector2f a =
            5.f * g * (r / pow(r2 + pow(d, 2.f), 1.5f)) * d * d;

        // clear the window with black color
        window.clear(sf::Color::Black);
        p.update(a, dt);

        // draw frame
        window.draw(rectangle);
        window.draw(p.getShape());
        window.draw(p2.getShape());

        // Set the view
        view.setCenter(p.r);
        window.setView(view);

        // end the current frame
        window.display();
    }
}
