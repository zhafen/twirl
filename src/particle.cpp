#include "particle.h"

Particle2::Particle2(sf::Vector2f r, sf::Vector2f v, float R) : sf::CircleShape(R), r(r), v(v) {
    // Set the origin as the center of the particle
    // setOrigin(R, R);
    // setPosition(r);
}

// void Particle::update(sf::Vector2f a, float dt) {
//     // Update using leapfrog algorithm
//     v += a * dt / 2.f;
//     r += v * dt;
//     v += a * dt / 2.f;
//     setPosition(r);
// }
// 
// void Particle::setPosition(sf::Vector2f pos) {
//     // r = pos;
//     sf::CircleShape::setPosition(pos);
// }