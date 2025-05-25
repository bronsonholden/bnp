#pragma once

#include <entt/entt.hpp>

namespace bnp {

	class Planet2DManager {
	public:
		Planet2DManager() = default;

		void update(entt::registry& registry, float dt);
	};

}
