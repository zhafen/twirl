#include "shape.h"

#include <SFML/Graphics.hpp>

#include "config.h"

namespace twirl {

CenteredCircleShape::CenteredCircleShape(float radius) : sf::CircleShape(radius) {
    // Set the origin to a new default (e.g., center of the circle)
    setOrigin(radius, radius);  // This changes the origin to the center of the circle
}

// NOTE: For some reason, for the origin to be set correctly it must be set once here
// and once after construction....
CenteredText::CenteredText() : sf::Text() {
    // center text
    sf::FloatRect text_rect = getLocalBounds();
    setOrigin(text_rect.left + text_rect.width / 2.0f,
              text_rect.top + text_rect.height / 2.0f);
    setFont(cfg.font);
}

}  // namespace twirl
