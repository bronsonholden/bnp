#pragma once

#include <entt/entt.hpp>

namespace bnp {

	// updates sprite animations within the given registry
	class SpriteAnimationManager {
	public:
		SpriteAnimationManager() = default;

		void update(entt::registry& registry, float dt);

	private:
		// changes animations/facing based on e.g. motility state
		void update_animations(entt::registry& registry, float dt);

		// update current animations with timestep
		void tick_animations(entt::registry& registry, float dt);

		// update 2d colliders if necessary
		void update_colliders(entt::registry& registry);
	};

}
