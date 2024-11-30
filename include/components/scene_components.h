#ifndef SCENE_COMPONENTS_H
#define SCENE_COMPONENTS_H

#include "components/base_components.h"
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct SceneComp {
    std::string scene_fp;
    bool emplace_after_loading = false;
    json json_data;
    size_t n_emplaced = 0;
    bool verbose_names = true;
};
inline void from_json(const json& j, SceneComp& scenecomp) {
    scenecomp.scene_fp = j.at("scene_fp").get<std::string>();
    scenecomp.emplace_after_loading = j.value("emplace_after_loading", false);
    scenecomp.verbose_names = j.value("verbose_names", true);
}

struct SceneTriggerComp {
    std::string scene_name;
    entt::entity scene_entity = entt::null;
    int n_triggers = 0;
};
inline void from_json(const json& j, SceneTriggerComp& scenetriggercomp) {
    scenetriggercomp.scene_name = j.at("scene_name").get<std::string>();
    scenetriggercomp.n_triggers = j.value("n_triggers", 0);
}

struct PairComp {
    entt::entity target_entity = entt::null;
    entt::entity source_entity = entt::null;
    EntityName target_entity_name = "";
    EntityName source_entity_name = "";
};
inline void from_json(const json& j, PairComp& paircomp) {
    paircomp.target_entity_name = j.value("target_entity_name", "");
    paircomp.source_entity_name = j.value("source_entity_name", "");
}

struct SyncPositionComp {
    bool once_only = false;
};
inline void from_json(const json& j, SyncPositionComp& syncpositioncomp) {
    syncpositioncomp.once_only = j.value("once_only", false);
}

}  // namespace twirl

#endif  // SCENE_COMPONENTS_H