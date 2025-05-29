#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace bnp {

	class UIManager {
	public:
		UIManager() = default;

		void update(entt::registry& registry, float dt);
		void update_mouse_position(entt::registry& registry, glm::vec2 worldspace_pos);
		void process_click(entt::registry& registry, glm::vec2 worldspace_pos);

		void update_hoverables(entt::registry& registry);
		void update_sprite_animators(entt::registry& registry);

	private:
		glm::vec2 mouse_worldspace_pos;
	};

}
