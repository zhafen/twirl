#include "game_objects.h"

#include <SFML/Graphics.hpp>

Config::Config()
    : window_size(1920u, 1080u),
      // Base scales
      L(0.01f * window_size.x),
      T(1.0f),
      V(L / T),
      A(V / T),
      health_rate(1.f / T),
      // By setting dt to not vary depending on how long it took to generate a frame,
      // our simulation slows down when it takes longer to render.
      fps(144u),
      dt(1.f / fps),
      dx(V * dt) {}

CCCircleShape::CCCircleShape(float radius) : sf::CircleShape(radius) {
    // Set the origin to a new default (e.g., center of the circle)
    setOrigin(radius, radius);  // This changes the origin to the center of the circle
}
