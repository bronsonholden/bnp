#pragma once

#include <bnp/components/physics.h>

#include <entt/entt.hpp>

namespace bnp {

// todo: add turbulence (random pockets of velocity change)
class Water2DManager {
public:
	float tension = 0.11f;
	float damping = 0.25f;
	float spread = 0.5f;
	int smoothing_passes = 5;

	void update(entt::registry& registry, float dt);
	void disturb(entt::registry& registry, entt::entity entity, int columnIndex, float velocity);
};

}
