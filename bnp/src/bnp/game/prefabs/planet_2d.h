#pragma once

#include <bnp/core/node.hpp>
#include <bnp/managers/resource_manager.h>

#include <entt/entt.hpp>

namespace bnp {
	namespace Prefab {

		Node planet_2d(entt::registry& registry, ResourceManager& resource_manager);

	}
}
