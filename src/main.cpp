#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

class Particle {
public:
    Particle(sf::Vector2f x, sf::Vector2f v, float r) : x(x), v(v), shape(r) {
        // shape.setFillColor(sf::Color(256, 256, 256));
        shape.setFillColor(sf::Color(255, 255, 255));
        shape.setPosition(x);
    }

    void update(sf::Vector2f a, float dt) {
        // Update using leapfrog algorithm
        v += a * dt / 2.f;
        x += v * dt;
        v += a * dt / 2.f;
    }

    sf::Vector2f x;
    sf::Vector2f v;
    float r;
    sf::CircleShape shape;
};

int main()
{
    auto window = sf::RenderWindow({1920u, 1080u}, "twirl");
    window.setFramerateLimit(144);

    sf::CircleShape shape(50.f);

    Particle p(sf::Vector2f(300.f, 300.f), sf::Vector2f(0.f, 0.f), 50.f);

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

        // draw frame
        window.draw(p.shape);

        // end the current frame
        window.display();
    }
}
