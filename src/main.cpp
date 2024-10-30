#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

class Particle {
public:
    Particle(sf::Vector2f x, sf::Vector2f v, float r) : x(x), v(v), shape(r) {
        // Set the origin as the center of the particle
        shape.setOrigin(r, r);
        shape.setPosition(x);
    }

    void update(sf::Vector2f a, float dt) {
        // Update using leapfrog algorithm
        v += a * dt / 2.f;
        x += v * dt;
        v += a * dt / 2.f;
    }

    sf::CircleShape getShape() const { return shape; }

private:
    sf::Vector2f x;
    sf::Vector2f v;
    float r;
    sf::CircleShape shape;
};

int main()
{
    // Window settings
    auto window = sf::RenderWindow({1920u, 1080u}, "twirl");
    int fps = 144;
    window.setFramerateLimit(fps);
    // By setting dt to not vary depending on how long it took to generate a frame,
    // our game slows down when it takes longer to render.
    float dt = 1.f / fps;
    sf::Vector2u window_size = window.getSize();
    // d will be our length scale, and we set it to a multiple of window size
    float d(0.01f * window_size.x);

    // Create and activate a view
    sf::View view(sf::Vector2f(0, 0), sf::Vector2f(window_size.x, window_size.y));
    window.setView(view);

    Particle p(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f), d);

    // Reference rectangle
    sf::RectangleShape rectangle(sf::Vector2f(d, d));
    rectangle.setFillColor(sf::Color(125, 125, 125));

    while (window.isOpen())
    {
        for (auto event = sf::Event(); window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // // Gravitational force
        // r = x2 - x;
        // float r2 = pow(r.x, 2.f) + pow(r.y, 2.f);
        // a = r / (pow(r2, 1.5f) + 10.f);

        // clear the window with black color
        window.clear(sf::Color::Black);
        p.update(sf::Vector2f(d, d), 100);

        // draw frame
        window.draw(rectangle);
        window.draw(p.getShape());

        // end the current frame
        window.display();
    }
}
