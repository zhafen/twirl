#include "components/component.h"

#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>

namespace twirl {

namespace comp {

entt::entity getEntityFromCompStr(entt::registry& registry,
                                  const std::string& comp_str) {
    return entt::null;
}

}  // namespace comp

}  // namespace twirl