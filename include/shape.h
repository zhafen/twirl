#ifndef CCSHAPE_H
#define CCSHAPE_H

#include <SFML/Graphics.hpp>

namespace twirl {

class CenteredCircleShape : public sf::CircleShape {
   public:
    // Constructor that sets a different default origin
    CenteredCircleShape(float radius = 50.f);
};

class CenteredText : public sf::Text {
   public:
    // Constructor that sets a different default origin
    CenteredText();
};

} // namespace twirl

#endif  // CCSHAPE_H