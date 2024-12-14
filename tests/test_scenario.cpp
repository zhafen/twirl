#include <gtest/gtest.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/helper.hpp>
#include <nlohmann/json.hpp>

#include "config.h"
#include "components/components.h"
#include "game.h"

using namespace twirl;

float MAX_TIME = 0.5f;

TEST(ScenarioTest, TestScene) {
    // Initialize the game, hidden in the constructor
    Game game("../../tests/test_data/test_scenes/main_test_scene.json");
    game.max_time = MAX_TIME;
    game.run();
}

TEST(ScenarioTest, TestMenuScene) {

    // DEBUG
    // create the window
    // sf::RenderWindow window(sf::VideoMode(1600, 900), "My window");

    // // run the program as long as the window is open
    // while (window.isOpen())
    // {
    //     // check all the window's events that were triggered since the last iteration of the loop
    //     sf::Event event;
    //     while (window.pollEvent(event))
    //     {
    //         // "close requested" event: we close the window
    //         if (event.type == sf::Event::Closed)
    //             window.close();
    //     }

    //     // clear the window with black color
    //     window.clear(sf::Color::Black);

    //     sf::Font font;
    //     font.loadFromFile("../../assets/arial.ttf");
    //     sf::Text text;
    //     text.setFont(font);
    //     text.setString("Hello, world!");
    //     text.setCharacterSize(24);
    //     text.setFillColor(sf::Color::Red);

    //     // draw everything here...
    //     window.draw(text);

    //     // end the current frame
    //     window.display();
    // }

    // Initialize the game, hidden in the constructor
    Game game("../../tests/test_data/test_scenes/test_menu.json");

    // DEBUG
    // center text
    auto& registry = game.getRegistry();
    auto& text_c = registry.get<TextComp>(registry.view<TextComp>().front());
    sf::FloatRect textRect = text_c.text.getLocalBounds();
    text_c.text.setOrigin(textRect.left + textRect.width / 2.0f,
              textRect.top + textRect.height / 2.0f);

    game.max_time = 5.0f;
    game.run();
}