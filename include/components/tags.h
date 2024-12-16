#ifndef TAGS_H
#define TAGS_H

#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct PlayerFlag {};

struct EnemyFlag {};

struct UnresolvedNameFlag {};

struct RenderFlag {};

struct UIRenderFlag {};

struct ViewFlag {};

struct DeleteFlag {};

struct SuspendedFlag {};

struct WatchTriggerFlag {};

struct DurabilityTriggerFlag {};

struct EnemyAbsenceTriggerFlag {};

}  // namespace twirl

#endif  // TAGS_H