#pragma once

#include <bnp/components/physics.h>

#include <entt/entt.hpp>

namespace bnp {

	class MotilityManager {
	public:
		MotilityManager() = default;

		void update(entt::registry& registry, float dt);
	};

}
