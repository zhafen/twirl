#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    auto window = sf::RenderWindow({1920u, 1080u}, "twirl");
    window.setFramerateLimit(144);

    sf::CircleShape shape(50.f);

    // Physical quantities
    sf::Vector2f a(0.f, 9.8f);
    sf::Vector2f v(0.f, 0.f);
    sf::Vector2f x(0.f, 0.f);
    float dt = 0.01f;

    // set the shape color to green
    shape.setFillColor(sf::Color(100, 250, 50));

    while (window.isOpen())
    {
        for (auto event = sf::Event(); window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // Update using leapfrog algorithm
        v += a * dt / 2.f;
        x += v * dt;
        v += a * dt / 2.f;

        shape.setPosition(x);

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw frame
        window.draw(shape);

        // end the current frame
        window.display();
    }
}
