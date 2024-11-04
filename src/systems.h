#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <SFML/Graphics.hpp>

#include "game_objects.h"

class RenderSystem {
   public:
    RenderSystem(const Config& cfg);

   private:
    Config cfg;
};

#endif  // SYSTEMS_H