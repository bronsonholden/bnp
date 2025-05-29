#include <bnp/core/logger.hpp>
#include <bnp/managers/ui_manager.h>
#include <bnp/components/ui.h>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>

namespace bnp {

	void UIManager::update(entt::registry& registry, float dt) {
		update_hoverables(registry);
		update_sprite_animators(registry);
	}

	void UIManager::update_mouse_position(entt::registry& registry, glm::vec2 worldspace_pos) {
		mouse_worldspace_pos = worldspace_pos;
	}

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


	void UIManager::update_hoverables(entt::registry& registry) {
		auto view = registry.view<Hoverable, Transform>();

		for (auto entity : view) {
			auto& transform = view.get<Transform>(entity);

			glm::vec2 bottom_left = glm::vec2(transform.position) - glm::vec2(transform.scale.x / 2, transform.scale.y / 2);
			glm::vec2 top_right = bottom_left + glm::vec2(transform.scale);

			if (mouse_worldspace_pos.x < bottom_left.x || mouse_worldspace_pos.y < bottom_left.y || mouse_worldspace_pos.x > top_right.x || mouse_worldspace_pos.y > top_right.y) {
				registry.patch<Hoverable>(entity, [](Hoverable& h) {
					h.hovered = false;
					});
			}
			else {
				registry.patch<Hoverable>(entity, [](Hoverable& h) {
					h.hovered = true;
					});
			}
		}
	}

	void UIManager::update_sprite_animators(entt::registry& registry) {
		auto view = registry.view<Hoverable, SpriteAnimator>();

		for (auto entity : view) {
			auto& hoverable = view.get<Hoverable>(entity);
			auto& animator = view.get<SpriteAnimator>(entity);

			if (hoverable.hovered) {
				animator.current_animation = "Hover";
			}
			else {
				animator.current_animation = "Default";
			}
		}
	}

}
