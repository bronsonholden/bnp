#include <bnp/core/logger.hpp>
#include <bnp/managers/ui_manager.h>
#include <bnp/components/ui.h>
#include <bnp/components/transform.h>

namespace bnp {

	void UIManager::process_click(entt::registry& registry, glm::vec2 worldspace_pos) {
		auto view = registry.view<Button, Transform>();

		for (auto entity : view) {
			auto& transform = view.get<Transform>(entity);

			glm::vec2 bottom_left = glm::vec2(transform.position) - glm::vec2(transform.scale.x / 2, transform.scale.y / 2);
			glm::vec2 top_right = bottom_left + glm::vec2(transform.scale);

			if (worldspace_pos.x < bottom_left.x || worldspace_pos.y < bottom_left.y || worldspace_pos.x > top_right.x || worldspace_pos.y > top_right.y) {
				continue;
			}

			Log::info("Clicked %d", (int)entity);

			return;
		}
	}

}
