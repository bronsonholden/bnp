#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace bnp {

	class UIManager {
	public:
		UIManager() = default;

		void process_click(entt::registry& registry, glm::vec2 worldspace_pos);
	};

}
