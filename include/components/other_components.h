#ifndef OTHER_COMPONENTS_H
#define OTHER_COMPONENTS_H

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct MouseButtonReleasedComp {};

}  // namespace twirl

#endif  // OTHER_COMPONENTS_H