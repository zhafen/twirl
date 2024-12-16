#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <SFML/Graphics.hpp>

namespace twirl {

using EntityId = int;
using EntityRelationId = int;

// OPTIMIZE: Consider computing these at compile time since they won't change.
struct Config {
    static constexpr uint16_t window_size_x = 1920u;
    static constexpr uint16_t window_size_y = 1080u;
    static constexpr float L = 0.01f * window_size_y;
    static constexpr float T = 1.0f;
    static constexpr float V = 0.1 * L / T;
    static constexpr float A = V / T;
    static constexpr float U = A * L;
    static constexpr uint16_t fps = 144;
    static constexpr float dt = 1.0f / fps;
    static constexpr float dx = V * dt;
    sf::Font font;
    Config() {
        font.loadFromFile("../../assets/arial.ttf");
    }
};

// Make global
inline Config cfg;

} // namespace twirl

#endif  // CONFIG_H