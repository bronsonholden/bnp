#pragma once

#include <bnp/components/physics.h>

#include <entt/entt.hpp>

namespace bnp {

	class Water2DManager {
	public:
		float tension = 0.12f;
		float damping = 0.3f;
		float spread = 0.5f;
		int smoothing_passes = 5;

		void update(entt::registry& registry, float dt);
		void disturb(entt::registry& registry, entt::entity entity, int columnIndex, float velocity);
	};

}
