#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include <SFML/Graphics.hpp>
#include "components/sfml_components.h"
#include <nlohmann/json.hpp>
#include "config.h"

using json = nlohmann::ordered_json;

namespace twirl {

struct MouseButtonReleasedComp {};

}  // namespace twirl

#endif  // UI_COMPONENTS_H