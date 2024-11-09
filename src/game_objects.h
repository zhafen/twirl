#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <SFML/Graphics.hpp>

// OPTIMIZE: Consider computing these at compile time since they won't change.
struct Config {
    static const uint16_t window_size_x = 1920u;
    static const uint16_t window_size_y = 1080u;
    static constexpr float L = 0.01f * window_size_x;
    static const float T = 1.0f;
    static constexpr float V = L / T;
    static constexpr float A = V / T;
    static const uint16_t fps = 144;
    static constexpr float dt = 1.0f / fps;
    static constexpr float dx = V * dt;
};

class CCCircleShape : public sf::CircleShape {
   public:
    // Constructor that sets a different default origin
    CCCircleShape(float radius = 50.f);
};

#endif  // GAMEOBJECTS_H