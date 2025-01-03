#ifndef ENTITY_H
#define ENTITY_H

#include <functional>
#include <memory>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "components/components.h"
#include "config.h"
#include "shape.h"

namespace twirl {

class EntitySystem {
   public:
    EntitySystem() = default;
    EntityMap& getEntityMap(entt::registry& registry);
    // TODO: deprecate or reimplement
    // entt::entity resolveEntityName(entt::registry& registry, EntityName name,
    //                                entt::entity entity);
    // void resolveEntityNames(entt::registry& registry);
    // void resolveSceneTriggerNames(entt::registry& registry);
    // void resolvePairNames(entt::registry& registry);
    void deleteEntities(entt::registry& registry);
    void syncEntities(entt::registry& registry);
    void orderEntities(entt::registry& registry);
    bool needs_ordering = true;

   private:
    EntityMap entity_map;
};

}  // namespace twirl

#endif  // ENTITY_H