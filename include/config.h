#ifndef CONFIG_H
#define CONFIG_H

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <iostream>

namespace twirl {

using EntityId = int;
using EntityRelationId = int;

struct Config {
    // Core game parameters
    static constexpr uint16_t window_size_x = 1920u;
    static constexpr uint16_t window_size_y = 1080u;
    static constexpr uint16_t fps = 144;
    // **Fundamental units**: everything can be broken down into mass, length, and time.
    // Mass is defined by the amount of acceleration an object incurs due to a
    //     standard force. In other words, the default mass is 1.0f.
    static constexpr float M = 1.0f;
    // Length is defined by the fraction of the screen a standard shape side takes up.
    // Defining L as 1% of the window height is convenient for choosing length
    // scales that are sufficiently small but also interpretable.
    static constexpr float L = 0.01f * window_size_y;
    // Time is the number of seconds per base time unit. The linking is done via the fps
    static constexpr float T = 1.0f;
    static constexpr float dt = T / fps;
    // Velocity. While everything can be put in terms of M, L, and T, we want speeds
    // (and accelerations) greater than L/T and L/T^2. For velocity I find it's
    // intuitive to set the percent of the screen that the object traverses
    // in one second. This gives a length scale H that we'll use for larger distances.
    static constexpr float H = 25.0f * L;
    static constexpr float V = H / T;
    // Acceleration. The same concept is used for acceleration, but now it's the
    // number of seconds to get up to speed V, so a measure of reactivity.
    static constexpr float P = 1.0f * T;
    static constexpr float A = V / P;
    // Force is just typical mass * acceleration
    static constexpr float F = M * A;
    // Energy is used in a few ways. We use KE to define the standard energy,
    // but there is also an energy that can be defined as work done, which is more
    // acceleration-based.
    static constexpr float E = 0.5f * M * V * V;
    // Remaining config constants.
    static constexpr float dx = V * dt;
    sf::Font font;
    Config() {
        font.loadFromFile("../../assets/arial.ttf"); 
        std::cout << "window_size_x: " << window_size_x << std::endl;
        std::cout << "window_size_y: " << window_size_y << std::endl;
        std::cout << "fps: " << fps << std::endl;
        std::cout << "M: " << M << std::endl;
        std::cout << "L: " << L << std::endl;
        std::cout << "T: " << T << std::endl;
        std::cout << "dt: " << dt << std::endl;
        std::cout << "H: " << H << std::endl;
        std::cout << "V: " << V << std::endl;
        std::cout << "P: " << P << std::endl;
        std::cout << "A: " << A << std::endl;
        std::cout << "E: " << E << std::endl;
        std::cout << "dx: " << dx << std::endl;
}
};

// Make global
inline Config cfg;

}  // namespace twirl

#endif  // CONFIG_H