#include "systems/interface.h"

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "components/components.h"
#include "config.h"
#include "shape.h"

namespace twirl {

bool InterfaceSystem::isGameSuspended(entt::registry& registry) {
    return (!registry.view<SuspendedFlag>().empty());
}

}  // namespace twirl
