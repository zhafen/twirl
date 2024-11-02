#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>

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

    void setPosition(sf::Vector2f pos) {
        r = pos;
        shape.setPosition(pos);
    }
    void move(sf::Vector2f dr) { setPosition(r + dr); }

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
    float dx(s * dt);

    // Create and activate a view
    sf::View view(sf::Vector2f(0, 0), sf::Vector2f(window_size.x, window_size.y));

    Particle p(sf::Vector2f(0.f, 20.f * d), sf::Vector2f(0.f, -s), d);

    Particle target(p.r, p.v, d / 2.f);
    target.shape.setFillColor(sf::Color::Black);
    target.shape.setOutlineThickness(d / 10.f);
    target.shape.setOutlineColor(sf::Color::White);
    sf::Vector2f r_pf(p.r);

    // Make some circles used for orientation
    std::vector<sf::CircleShape> circles(100);
    for (int i = 0; i < circles.size(); ++i) {
        sf::CircleShape cir_i(10.f * d * i);
        cir_i.setFillColor(sf::Color(127, 127, 127));
        cir_i.setOutlineThickness(d / 5.f);
        cir_i.setOutlineColor(sf::Color(63, 63, 63));
        cir_i.setOrigin(cir_i.getRadius(), cir_i.getRadius());
        circles[circles.size() - i - 1] = cir_i;
    }

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

            if (event.type == sf::Event::MouseButtonPressed) {
                // get the current mouse position in the window
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                // convert it to world coordinates
                target.r = window.mapPixelToCoords(pixelPos);
                target.shape.setPosition(target.r);
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

        // Gravitational force
        sf::Vector2f r = target.r - p.r;
        float r2 = pow(r.x, 2.f) + pow(r.y, 2.f);
        sf::Vector2f a = 5.f * g * (r / pow(r2 + pow(d, 2.f), 1.5f)) * d * d;

        // clear the window with black color
        window.clear(sf::Color::Black);
        p.update(a, dt);

        // draw frame
        for (int i = 0; i < circles.size(); ++i) {
            window.draw(circles[i]);
        }
        // window.draw(rectangle);
        window.draw(p.getShape());
        window.draw(target.getShape());

        // Set the view
        view.setCenter(p.r);
        window.setView(view);

        // end the current frame
        window.display();
    }
}
