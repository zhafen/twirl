#ifndef TAGS_H
#define TAGS_H

#include <nlohmann/json.hpp>
#include "config.h"
#include <nlohmann/json.hpp>
#include "config.h"
#include <nlohmann/json.hpp>
#include "config.h"
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct PlayerFlag {};

struct EnemyFlag {};

struct UnresolvedNameFlag {};

struct ViewFlag {};

struct DeleteFlag {};

struct WatchTriggerFlag {};

}  // namespace twirl

#endif  // TAGS_H