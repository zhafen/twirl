#include <SFML/Graphics.hpp>
#include "shape.h"
#include <gtest/gtest.h>

// Example test case for shape-related functions
TEST(CenteredCircleShapeTest, BasicTest) {
    float radius = 50.f;
    twirl::CenteredCircleShape circle(radius);

    // Check if the origin is set to the center of the circle
    sf::Vector2f origin = circle.getOrigin();
    assert(origin.x == radius);
    assert(origin.y == radius);
}