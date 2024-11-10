#include "game.h"

#include <iostream>
#include <memory>

struct SecondStruct {
    float value;
};

struct FirstStruct {
    std::shared_ptr<float> valuePtr; // Shared pointer to a float
};

int main() {
    cc::Game game;
    game.run();
    return 0;
}
