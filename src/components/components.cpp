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
    if (comp_key == "PlayerComp") {
        registry.emplace<PlayerComp>(entity);
    } else if (comp_key == "EnemyComp") {
        registry.emplace<EnemyComp>(entity);
    } else if (comp_key == "UnresolvedNameComp") {
        registry.emplace<UnresolvedNameComp>(entity);
    } else if (comp_key == "ViewComp") {
        registry.emplace<ViewComp>(entity);
    } else if (comp_key == "DeleteComp") {
        registry.emplace<DeleteComp>(entity);
    } else if (comp_key == "SceneComp") {
        auto scenecomp = comp_json.template get<SceneComp>();
        registry.emplace<SceneComp>(entity, scenecomp);
    } else if (comp_key == "SceneTriggerComp") {
        auto scenetriggercomp = comp_json.template get<SceneTriggerComp>();
        registry.emplace<SceneTriggerComp>(entity, scenetriggercomp);
    } else if (comp_key == "PairComp") {
        auto paircomp = comp_json.template get<PairComp>();
        registry.emplace<PairComp>(entity, paircomp);
    } else if (comp_key == "SyncPositionComp") {
        auto syncpositioncomp = comp_json.template get<SyncPositionComp>();
        registry.emplace<SyncPositionComp>(entity, syncpositioncomp);
    } else if (comp_key == "StopWatchComp") {
        auto stopwatchcomp = comp_json.template get<StopWatchComp>();
        registry.emplace<StopWatchComp>(entity, stopwatchcomp);
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
    } else if (comp_key == "RenderComp") {
        auto rendercomp = comp_json.template get<RenderComp>();
        registry.emplace<RenderComp>(entity, rendercomp);
    } else if (comp_key == "UIComp") {
        auto uicomp = comp_json.template get<UIComp>();
        registry.emplace<UIComp>(entity, uicomp);
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
    if (comp_key == "PlayerComp") {
        return getEntityFromSelectionStr<PlayerComp>(registry, selection_str);
    } else if (comp_key == "EnemyComp") {
        return getEntityFromSelectionStr<EnemyComp>(registry, selection_str);
    } else if (comp_key == "UnresolvedNameComp") {
        return getEntityFromSelectionStr<UnresolvedNameComp>(registry, selection_str);
    } else if (comp_key == "ViewComp") {
        return getEntityFromSelectionStr<ViewComp>(registry, selection_str);
    } else if (comp_key == "DeleteComp") {
        return getEntityFromSelectionStr<DeleteComp>(registry, selection_str);
    } else if (comp_key == "SceneComp") {
        return getEntityFromSelectionStr<SceneComp>(registry, selection_str);
    } else if (comp_key == "SceneTriggerComp") {
        return getEntityFromSelectionStr<SceneTriggerComp>(registry, selection_str);
    } else if (comp_key == "PairComp") {
        return getEntityFromSelectionStr<PairComp>(registry, selection_str);
    } else if (comp_key == "SyncPositionComp") {
        return getEntityFromSelectionStr<SyncPositionComp>(registry, selection_str);
    } else if (comp_key == "StopWatchComp") {
        return getEntityFromSelectionStr<StopWatchComp>(registry, selection_str);
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
    } else if (comp_key == "RenderComp") {
        return getEntityFromSelectionStr<RenderComp>(registry, selection_str);
    } else if (comp_key == "UIComp") {
        return getEntityFromSelectionStr<UIComp>(registry, selection_str);
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
        }
        
        auto scenetriggercomp_ptr = registry.try_get<SceneTriggerComp>(entity);
        if (scenetriggercomp_ptr != nullptr) {
            auto scenetriggercomp = *scenetriggercomp_ptr;
            std::cout << "    SceneTriggerComp" << std::endl;
            std::cout << "      scene_name: " << scenetriggercomp.scene_name << std::endl;
            std::cout << "      n_triggers: " << scenetriggercomp.n_triggers << std::endl;
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
        
        auto stopwatchcomp_ptr = registry.try_get<StopWatchComp>(entity);
        if (stopwatchcomp_ptr != nullptr) {
            auto stopwatchcomp = *stopwatchcomp_ptr;
            std::cout << "    StopWatchComp" << std::endl;
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
        
        
        
        auto rendercomp_ptr = registry.try_get<RenderComp>(entity);
        if (rendercomp_ptr != nullptr) {
            auto rendercomp = *rendercomp_ptr;
            std::cout << "    RenderComp" << std::endl;
            std::cout << "      zorder: " << rendercomp.zorder << std::endl;
        }
        
        auto uicomp_ptr = registry.try_get<UIComp>(entity);
        if (uicomp_ptr != nullptr) {
            auto uicomp = *uicomp_ptr;
            std::cout << "    UIComp" << std::endl;
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