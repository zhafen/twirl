#ifndef CCSHAPE_H
#define CCSHAPE_H

#include <SFML/Graphics.hpp>

namespace twirl {

class TwirlCircleShape : public sf::CircleShape {
   public:
    // Constructor that sets a different default origin
    TwirlCircleShape(float radius = 50.f);
};

} // namespace twirl

#endif  // CCSHAPE_H