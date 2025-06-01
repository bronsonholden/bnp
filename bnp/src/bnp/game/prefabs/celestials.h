#pragma once

#include <bnp/core/node.hpp>
#include <bnp/managers/resource_manager.h>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Prefab {
namespace Celestials {

Node celestial_orbit(entt::registry& registry, ResourceManager& resource_manager);
Node galaxy(entt::registry& registry, ResourceManager& resource_manager);
Node galaxy_map_dot(entt::registry& registry, ResourceManager& resource_manager);
Node celestial(entt::registry& registry, ResourceManager& resource_manager);

}
}
}
}
