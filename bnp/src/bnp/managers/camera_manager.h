#pragma once

#include <bnp/components/world.h>

#include <entt/entt.hpp>

namespace bnp {

	class CameraManager {
	public:
		CameraManager() = default;

		void update(entt::registry& registry, float dt);
	};

}
