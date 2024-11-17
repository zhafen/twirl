#include "game.h"

#include <iostream>
#include <memory>

int main() {
    twirl::Game game("../../scenes/main_scene.json");
    game.run();
    return 0;
}
