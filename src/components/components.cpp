#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include "config.h"
#include "components/components.h"

using json = nlohmann::ordered_json;

namespace twirl {

void emplaceComponent(entt::registry& registry, entt::entity entity,
                      const std::string& comp_key, const json& comp_json) {
    if (comp_key == "PlayerComp") {
        registry.emplace<PlayerComp>(entity);
    } else if (comp_key == "EnemyComp") {
        registry.emplace<EnemyComp>(entity);
    } else if (comp_key == "ViewComp") {
        registry.emplace<ViewComp>(entity);
    } else if (comp_key == "DeleteComp") {
        registry.emplace<DeleteComp>(entity);
    } else if (comp_key == "DebugComp") {
        registry.emplace<DebugComp>(entity);
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
    } else {
        throw std::runtime_error("Unknown component type");
    }
}
}  // namespace twirl