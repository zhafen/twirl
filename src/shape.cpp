#include "shape.h"

#include <SFML/Graphics.hpp>

#include "config.h"

namespace twirl {

CenteredCircleShape::CenteredCircleShape(float radius) : sf::CircleShape(radius) {
    // Set the origin to a new default (e.g., center of the circle)
    setOrigin(radius, radius);  // This changes the origin to the center of the circle
}

CenteredText::CenteredText() : sf::Text() {
    // center text
    sf::FloatRect textRect = getLocalBounds();
    setOrigin(textRect.left + textRect.width / 2.0f,
              textRect.top + textRect.height / 2.0f);
    setFont(cfg.font);
}

}  // namespace twirl
