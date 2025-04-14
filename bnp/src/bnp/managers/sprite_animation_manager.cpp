#include <bnp/managers/sprite_animation_manager.h>
#include <bnp/components/graphics.h>
#include <bnp/components/physics.h>

namespace  bnp {

	void SpriteAnimationManager::update(entt::registry& registry, float dt) {
		update_animations(registry, dt);
		tick_animations(registry, dt);
	}

	void SpriteAnimationManager::update_animations(entt::registry& registry, float dt) {
		auto view = registry.view<Sprite, SpriteAnimator, Motility>();

		for (auto entity : view) {
			auto& sprite = registry.get<Sprite>(entity);
			auto& animator = registry.get<SpriteAnimator>(entity);
			auto& motility = registry.get<Motility>(entity);

			registry.patch<Sprite>(entity, [&](Sprite& sprite) {
				if (glm::length(motility.current_velocity) > 0.001f) {
					if (motility.current_velocity.x < 0) {
						sprite.mirror.x = -1;
					}
					else {
						sprite.mirror.x = 1;
					}
				}
				});

			registry.patch<SpriteAnimator>(entity, [&motility](SpriteAnimator& animator) {
				std::string new_animation = animator.current_animation;

				if (motility.walking) {
					new_animation = "Walk";
				}
				else if (motility.idle) {
					new_animation = "Idle";
				}

				if (new_animation != animator.current_animation) {
					animator.current_frame_index = 0;
					animator.time = 0;
					animator.current_animation = new_animation;
				}
				});
		}
	}

	void SpriteAnimationManager::tick_animations(entt::registry& registry, float dt) {
		auto view = registry.view<Sprite, SpriteAnimator>();

		for (auto entity : view) {
			auto& animator = view.get<SpriteAnimator>(entity);

			if (!animator.playing) {
				continue;
			}

			auto& sprite = view.get<Sprite>(entity);
			const auto& animation = sprite.animations.at(animator.current_animation);
			const auto& frame = animation.frames.at(animator.current_frame_index);

			size_t new_frame_index = animator.current_frame_index;
			float new_time = animator.time + dt;
			bool new_done = false;

			if (new_time > frame.duration) {
				new_time -= frame.duration;
				new_frame_index += 1;
			}

			if (new_frame_index >= animation.frames.size()) {
				if (animator.loop) {
					new_frame_index = 0;
				}
				else {
					new_done = true;
					new_frame_index = animation.frames.size() - 1;
				}
			}

			registry.patch<SpriteAnimator>(entity, [=](SpriteAnimator& sa) {
				sa.current_frame_index = new_frame_index;
				sa.time = new_time;
				sa.done = new_done;
				});
		}
	}

}
