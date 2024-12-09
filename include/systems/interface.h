#ifndef INTERFACE_H
#define INTERFACE_H

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "components/components.h"
#include "config.h"

namespace twirl {

class InterfaceSystem {
   public:
    InterfaceSystem() = default;
    bool isGameSuspended(entt::registry& registry);
};

} // namespace twirl

#endif  // INTERFACE_H