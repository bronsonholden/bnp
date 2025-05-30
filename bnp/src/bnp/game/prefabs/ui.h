#pragma once

#include <bnp/core/node.hpp>
#include <bnp/managers/resource_manager.h>

#include <entt/entt.hpp>

namespace bnp {
namespace Prefab {
namespace UI {

Node play_button(entt::registry& registry, ResourceManager& resource_manager);

}
}
}
