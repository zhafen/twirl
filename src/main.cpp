#include <SFML/Graphics.hpp>

int main()
{
    auto window = sf::RenderWindow({1920u, 1080u}, "twirl");
    window.setFramerateLimit(144);

    sf::CircleShape shape(50.f);

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

        // clear the window with black color
        window.clear(sf::Color::Black);

        window.draw(shape);

        // end the current frame
        window.display();
    }
}
