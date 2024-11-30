#ifndef SFML_COMPONENTS_H
#define SFML_COMPONENTS_H

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

// from_json functions for sfml types
namespace sf {
inline void from_json(const json& j, sf::Vector2f& v) {
    v.x = j.at(0).get<float>();
    v.y = j.at(1).get<float>();
}
inline void from_json(const json& j, sf::Color& c) {
    c.r = j.at(0).get<uint8_t>();
    c.g = j.at(1).get<uint8_t>();
    c.b = j.at(2).get<uint8_t>();
    c.a = j.at(3).get<uint8_t>();
}
}  // namespace sf

#endif // SFML_COMPONENTS_H