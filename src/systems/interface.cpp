#include "systems/interface.h"

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "components/components.h"
#include "config.h"
#include "shape.h"

namespace twirl {

bool InterfaceSystem::isGameSuspended(entt::registry& registry) {
    // Check for various scenarios
    bool game_suspended = false;

    // Is the player dead?
    entt::entity player_entity = registry.view<PlayerComp>().front();
    auto durability_c = registry.get<DurabilityComp>(player_entity);
    game_suspended = game_suspended || (durability_c.durability <= 0);

    // Are all the enemies dead?
    auto enemy_view = registry.view<EnemyComp>();
    game_suspended = game_suspended || enemy_view.empty();

    return game_suspended;
}

}  // namespace twirl
