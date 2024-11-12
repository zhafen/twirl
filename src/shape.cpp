#include "shape.h"

#include <SFML/Graphics.hpp>

namespace twirl {

TwirlCircleShape::TwirlCircleShape(float radius) : sf::CircleShape(radius) {
    // Set the origin to a new default (e.g., center of the circle)
    setOrigin(radius, radius);  // This changes the origin to the center of the circle
}

}  // namespace twirl
