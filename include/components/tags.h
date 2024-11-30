#ifndef TAGS_H
#define TAGS_H

#include <nlohmann/json.hpp>
#include "config.h"
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct PlayerComp {};

struct EnemyComp {};

struct ViewComp {};

struct DeleteComp {};

struct DebugComp {};

}  // namespace twirl

#endif  // TAGS_H