#ifndef CCSHAPE_H
#define CCSHAPE_H

#include <SFML/Graphics.hpp>

namespace cc {

class CCCircleShape : public sf::CircleShape {
   public:
    // Constructor that sets a different default origin
    CCCircleShape(float radius = 50.f);
};

} // namespace cc

#endif  // CCSHAPE_H