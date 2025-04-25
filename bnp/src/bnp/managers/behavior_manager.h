#pragma once

#include <bnp/components/behavior.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace bnp {

	class BehaviorManager {
	public:
		BehaviorManager() = default;

		void update(entt::registry& registry, float dt);
		glm::vec2 smooth_direction(int x, int y, const std::vector<float>& cost_field, glm::ivec2 grid_size);
		void generate_direction_field(entt::registry& registry, FlowField2D& field);
		void generate_cost_field(entt::registry& registry, FlowField2D& field);
	};

}
