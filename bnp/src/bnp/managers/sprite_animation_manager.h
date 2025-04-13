#pragma once

#include <entt/entt.hpp>

namespace bnp {

	// updates sprite animations within the given registry
	class SpriteAnimationManager {
	public:
		SpriteAnimationManager() = default;

		void update(entt::registry& registry, float dt);
	};

}
