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

    // DEBUG
    // // Is the player dead? (if there is a player)
    // auto rview = registry.view<PlayerFlag>();
    // if (!rview.empty()) {
    //     entt::entity player_entity = rview.front();
    //     auto durability_c = registry.get<DurabilityComp>(player_entity);
    //     game_suspended = game_suspended || (durability_c.durability <= 0);
    // }

    // // Are all the enemies dead?
    // auto enemy_view = registry.view<EnemyFlag>();
    // game_suspended = game_suspended || enemy_view.empty();

    // return game_suspended;
}

}  // namespace twirl
