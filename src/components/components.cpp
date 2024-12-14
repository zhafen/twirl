#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include "components/components.h"
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

namespace comp {

void emplaceComponent(entt::registry& registry, entt::entity entity,
                      const std::string& comp_key, const json& comp_json) {
    if (comp_key == "PlayerFlag") {
        registry.emplace<PlayerFlag>(entity);
    } else if (comp_key == "EnemyFlag") {
        registry.emplace<EnemyFlag>(entity);
    } else if (comp_key == "UnresolvedNameFlag") {
        registry.emplace<UnresolvedNameFlag>(entity);
    } else if (comp_key == "ViewFlag") {
        registry.emplace<ViewFlag>(entity);
    } else if (comp_key == "DeleteFlag") {
        registry.emplace<DeleteFlag>(entity);
    } else if (comp_key == "SuspendedFlag") {
        registry.emplace<SuspendedFlag>(entity);
    } else if (comp_key == "WatchTriggerFlag") {
        registry.emplace<WatchTriggerFlag>(entity);
    } else if (comp_key == "SceneComp") {
        auto scenecomp = comp_json.template get<SceneComp>();
        registry.emplace<SceneComp>(entity, scenecomp);
    } else if (comp_key == "TriggerComp") {
        auto triggercomp = comp_json.template get<TriggerComp>();
        registry.emplace<TriggerComp>(entity, triggercomp);
    } else if (comp_key == "PairComp") {
        auto paircomp = comp_json.template get<PairComp>();
        registry.emplace<PairComp>(entity, paircomp);
    } else if (comp_key == "SyncPositionComp") {
        auto syncpositioncomp = comp_json.template get<SyncPositionComp>();
        registry.emplace<SyncPositionComp>(entity, syncpositioncomp);
    } else if (comp_key == "WatchComp") {
        auto watchcomp = comp_json.template get<WatchComp>();
        registry.emplace<WatchComp>(entity, watchcomp);
    } else if (comp_key == "PhysicsComp") {
        auto physicscomp = comp_json.template get<PhysicsComp>();
        registry.emplace<PhysicsComp>(entity, physicscomp);
    } else if (comp_key == "DragForceComp") {
        auto dragforcecomp = comp_json.template get<DragForceComp>();
        registry.emplace<DragForceComp>(entity, dragforcecomp);
    } else if (comp_key == "DurabilityComp") {
        auto durabilitycomp = comp_json.template get<DurabilityComp>();
        registry.emplace<DurabilityComp>(entity, durabilitycomp);
    } else if (comp_key == "PairwiseForceComp") {
        auto pairwiseforcecomp = comp_json.template get<PairwiseForceComp>();
        registry.emplace<PairwiseForceComp>(entity, pairwiseforcecomp);
    } else if (comp_key == "CollisionComp") {
        registry.emplace<CollisionComp>(entity);
    } else if (comp_key == "MouseButtonReleasedComp") {
        registry.emplace<MouseButtonReleasedComp>(entity);
    } else if (comp_key == "CircleComp") {
        auto circlecomp = comp_json.template get<CircleComp>();
        registry.emplace<CircleComp>(entity, circlecomp);
    } else if (comp_key == "RectangleComp") {
        auto rectanglecomp = comp_json.template get<RectangleComp>();
        registry.emplace<RectangleComp>(entity, rectanglecomp);
    } else if (comp_key == "ValueBarComp") {
        auto valuebarcomp = comp_json.template get<ValueBarComp>();
        registry.emplace<ValueBarComp>(entity, valuebarcomp);
    } else if (comp_key == "TextComp") {
        auto textcomp = comp_json.template get<TextComp>();
        registry.emplace<TextComp>(entity, textcomp);
    } else if (comp_key == "DebugComp") {
        auto debugcomp = comp_json.template get<DebugComp>();
        registry.emplace<DebugComp>(entity, debugcomp);
    } else {
        throw std::runtime_error("Unknown component type: " + comp_key);
    }
}

entt::entity getEntityFromStr(entt::registry& registry, const std::string& input_str) {
    // Parse the input string
    size_t colon = input_str.find('|');
    if (colon == std::string::npos) {
        throw std::runtime_error("Pipe not found in component string");
    }
    std::string comp_key = input_str.substr(0, colon);
    std::string selection_str = input_str.substr(colon + 1);

    // Get the view based on the component string
    if (comp_key == "EntityName") {
        return getEntityFromSelectionStr<EntityName>(registry, selection_str);
    } else if (comp_key == "PlayerFlag") {
        return getEntityFromSelectionStr<PlayerFlag>(registry, selection_str);
    } else if (comp_key == "EnemyFlag") {
        return getEntityFromSelectionStr<EnemyFlag>(registry, selection_str);
    } else if (comp_key == "UnresolvedNameFlag") {
        return getEntityFromSelectionStr<UnresolvedNameFlag>(registry, selection_str);
    } else if (comp_key == "ViewFlag") {
        return getEntityFromSelectionStr<ViewFlag>(registry, selection_str);
    } else if (comp_key == "DeleteFlag") {
        return getEntityFromSelectionStr<DeleteFlag>(registry, selection_str);
    } else if (comp_key == "SuspendedFlag") {
        return getEntityFromSelectionStr<SuspendedFlag>(registry, selection_str);
    } else if (comp_key == "WatchTriggerFlag") {
        return getEntityFromSelectionStr<WatchTriggerFlag>(registry, selection_str);
    } else if (comp_key == "SceneComp") {
        return getEntityFromSelectionStr<SceneComp>(registry, selection_str);
    } else if (comp_key == "TriggerComp") {
        return getEntityFromSelectionStr<TriggerComp>(registry, selection_str);
    } else if (comp_key == "PairComp") {
        return getEntityFromSelectionStr<PairComp>(registry, selection_str);
    } else if (comp_key == "SyncPositionComp") {
        return getEntityFromSelectionStr<SyncPositionComp>(registry, selection_str);
    } else if (comp_key == "WatchComp") {
        return getEntityFromSelectionStr<WatchComp>(registry, selection_str);
    } else if (comp_key == "PhysicsComp") {
        return getEntityFromSelectionStr<PhysicsComp>(registry, selection_str);
    } else if (comp_key == "DragForceComp") {
        return getEntityFromSelectionStr<DragForceComp>(registry, selection_str);
    } else if (comp_key == "DurabilityComp") {
        return getEntityFromSelectionStr<DurabilityComp>(registry, selection_str);
    } else if (comp_key == "PairwiseForceComp") {
        return getEntityFromSelectionStr<PairwiseForceComp>(registry, selection_str);
    } else if (comp_key == "CollisionComp") {
        return getEntityFromSelectionStr<CollisionComp>(registry, selection_str);
    } else if (comp_key == "MouseButtonReleasedComp") {
        return getEntityFromSelectionStr<MouseButtonReleasedComp>(registry, selection_str);
    } else if (comp_key == "CircleComp") {
        return getEntityFromSelectionStr<CircleComp>(registry, selection_str);
    } else if (comp_key == "RectangleComp") {
        return getEntityFromSelectionStr<RectangleComp>(registry, selection_str);
    } else if (comp_key == "ValueBarComp") {
        return getEntityFromSelectionStr<ValueBarComp>(registry, selection_str);
    } else if (comp_key == "TextComp") {
        return getEntityFromSelectionStr<TextComp>(registry, selection_str);
    } else if (comp_key == "DebugComp") {
        return getEntityFromSelectionStr<DebugComp>(registry, selection_str);
    } else {
        throw std::runtime_error("Unknown component type: " + comp_key);
    }
}

void debugEntities(entt::registry& registry, std::string message) {
    auto rview = registry.view<DebugComp>();
    bool message_printed = false;
    for (auto [entity, debug_c] : rview.each()) {
        if (!message_printed) {
            std::cout << message << std::endl;
            message_printed = true;
        }
        std::cout << "  entity: " << static_cast<int>(entity) << std::endl;
        auto entity_name_ptr = registry.try_get<EntityName>(entity);
        if (entity_name_ptr != nullptr) {
            std::cout << "  Entity Name: " << *entity_name_ptr << std::endl;
        }

        // Skip the rest if not verbose
        if (!debug_c.verbose) {
            continue;
        }
        
        
        
        
        
        
        
        auto scenecomp_ptr = registry.try_get<SceneComp>(entity);
        if (scenecomp_ptr != nullptr) {
            auto scenecomp = *scenecomp_ptr;
            std::cout << "    SceneComp" << std::endl;
            std::cout << "      scene_fp: " << scenecomp.scene_fp << std::endl;
            std::cout << "      emplace_order: " << scenecomp.emplace_order << std::endl;
        }
        
        auto triggercomp_ptr = registry.try_get<TriggerComp>(entity);
        if (triggercomp_ptr != nullptr) {
            auto triggercomp = *triggercomp_ptr;
            std::cout << "    TriggerComp" << std::endl;
            std::cout << "      n_triggers: " << triggercomp.n_triggers << std::endl;
        }
        
        auto paircomp_ptr = registry.try_get<PairComp>(entity);
        if (paircomp_ptr != nullptr) {
            auto paircomp = *paircomp_ptr;
            std::cout << "    PairComp" << std::endl;
        }
        
        auto syncpositioncomp_ptr = registry.try_get<SyncPositionComp>(entity);
        if (syncpositioncomp_ptr != nullptr) {
            auto syncpositioncomp = *syncpositioncomp_ptr;
            std::cout << "    SyncPositionComp" << std::endl;
        }
        
        auto watchcomp_ptr = registry.try_get<WatchComp>(entity);
        if (watchcomp_ptr != nullptr) {
            auto watchcomp = *watchcomp_ptr;
            std::cout << "    WatchComp" << std::endl;
        }
        
        auto physicscomp_ptr = registry.try_get<PhysicsComp>(entity);
        if (physicscomp_ptr != nullptr) {
            auto physicscomp = *physicscomp_ptr;
            std::cout << "    PhysicsComp" << std::endl;
        }
        
        auto dragforcecomp_ptr = registry.try_get<DragForceComp>(entity);
        if (dragforcecomp_ptr != nullptr) {
            auto dragforcecomp = *dragforcecomp_ptr;
            std::cout << "    DragForceComp" << std::endl;
        }
        
        auto durabilitycomp_ptr = registry.try_get<DurabilityComp>(entity);
        if (durabilitycomp_ptr != nullptr) {
            auto durabilitycomp = *durabilitycomp_ptr;
            std::cout << "    DurabilityComp" << std::endl;
        }
        
        auto pairwiseforcecomp_ptr = registry.try_get<PairwiseForceComp>(entity);
        if (pairwiseforcecomp_ptr != nullptr) {
            auto pairwiseforcecomp = *pairwiseforcecomp_ptr;
            std::cout << "    PairwiseForceComp" << std::endl;
        }
        
        
        
        auto circlecomp_ptr = registry.try_get<CircleComp>(entity);
        if (circlecomp_ptr != nullptr) {
            auto circlecomp = *circlecomp_ptr;
            std::cout << "    CircleComp" << std::endl;
            std::cout << "      zorder: " << circlecomp.zorder << std::endl;
        }
        
        auto rectanglecomp_ptr = registry.try_get<RectangleComp>(entity);
        if (rectanglecomp_ptr != nullptr) {
            auto rectanglecomp = *rectanglecomp_ptr;
            std::cout << "    RectangleComp" << std::endl;
        }
        
        auto valuebarcomp_ptr = registry.try_get<ValueBarComp>(entity);
        if (valuebarcomp_ptr != nullptr) {
            auto valuebarcomp = *valuebarcomp_ptr;
            std::cout << "    ValueBarComp" << std::endl;
        }
        
        auto textcomp_ptr = registry.try_get<TextComp>(entity);
        if (textcomp_ptr != nullptr) {
            auto textcomp = *textcomp_ptr;
            std::cout << "    TextComp" << std::endl;
            std::cout << "      string: " << textcomp.string << std::endl;
        }
        
        auto debugcomp_ptr = registry.try_get<DebugComp>(entity);
        if (debugcomp_ptr != nullptr) {
            auto debugcomp = *debugcomp_ptr;
            std::cout << "    DebugComp" << std::endl;
        }
    }
}

}  // namespace comp
}  // namespace twirl