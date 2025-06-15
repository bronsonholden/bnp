#pragma once

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Queries {

std::vector<entt::entity> get_item_entities_sorted_by_id(entt::registry& registry);

}
}
}
