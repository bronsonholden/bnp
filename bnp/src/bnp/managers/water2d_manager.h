#pragma once

#include <bnp/components/physics.h>

#include <entt/entt.hpp>

namespace bnp {

	class Water2DManager {
	public:
		float tension = 3.0f;
		float damping = 0.05f;
		float spread = 0.75f;
		int smoothing_passes = 8;

		void update(entt::registry& registry, float dt);
		void disturb(entt::registry& registry, entt::entity entity, int columnIndex, float velocity);
	};


}
