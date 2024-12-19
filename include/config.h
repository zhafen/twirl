#ifndef CONFIG_H
#define CONFIG_H

#include <SFML/Graphics.hpp>
#include <cstdint>

namespace twirl {

using EntityId = int;
using EntityRelationId = int;

// OPTIMIZE: Consider computing these at compile time since they won't change.
struct Config {
    static constexpr uint16_t window_size_x = 1920u;
    static constexpr uint16_t window_size_y = 1080u;
    // We define our units using four intuitive quantities
    // l, the fraction of the screen a standard shape takes up. (L = l * window size)
    // h, the fraction of the screen covered by a standard velocity in one second (H = h * window size)
    // p, time it takes to get up to a standard velocity (P = p * T)
    // d, distance it takes to get up to a standard velocity (D = d * H)
    // TODO: Determine how to link T to fps s.t. T is 1 second.
    static constexpr float L = 0.01f * window_size_y;
    // Length. Defining L as 1% of the window height is convenient for choosing length
    // scales that are sufficiently small but also interpretable.
    static constexpr float L = 0.01f * window_size_y;
    // Time, in seconds
    static constexpr float T = 1.0f;
    // Velocity. For velocity I find it's more intuitive to set the percent, of the screen
    // that the object traverses in one second. This gives a length scale H
    // that we'll use for larger distances
    static constexpr float H = 25.0f * L;
    static constexpr float V = H / T;
    // Acceleration. The same concept is used for acceleration, but now it's the
    // number of seconds to get up to speed V, so a measure of the speed the game
    // plays at.
    static constexpr float P = 2.0f * T;
    static constexpr float A = V / P;
    // Energy.
    static constexpr float E = 0.5f * M * V * V;
    // We have another length scale, which is the distance it took to reach speed V
    // given a standard force.
    static constexpr float D = 1.0f * H;
    // With H, P, and D defined, that constrains what M can be for those intuitive
    // quantities to be consistent.
    static constexpr uint16_t fps = 144;
    static constexpr float dt = 1.0f / fps;
    static constexpr float dx = V * dt;
    sf::Font font;
    Config() { font.loadFromFile("../../assets/arial.ttf"); }
};

// Make global
inline Config cfg;

}  // namespace twirl

#endif  // CONFIG_H