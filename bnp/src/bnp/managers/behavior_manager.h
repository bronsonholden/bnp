#pragma once

#include <bnp/managers/physics_manager.h>
#include <bnp/components/behavior.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace bnp {

	// todo: move flow field management to its own manager?
	class BehaviorManager {
	public:
		BehaviorManager(PhysicsManager& hysics_manager);
		~BehaviorManager();

		void update(entt::registry& registry, float dt);

		// set target to parent position
		void update_targets(entt::registry& registry, float dt);

		void regenerate_if_stale(entt::registry& registry, float dt);

	private:
		PhysicsManager& physics_manager;
	};

}
