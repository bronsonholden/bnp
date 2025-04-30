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
		glm::vec2 smooth_direction(int x, int y, const std::vector<float>& cost_field, glm::ivec2 grid_size, bool attract);
		void generate_direction_field(entt::registry& registry, FlowField2D& field);
		void generate_cost_field(entt::registry& registry, FlowField2D& field);

	private:
		PhysicsManager& physics_manager;
	};

}
