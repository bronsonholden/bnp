#include <bnp/managers/sprite_animation_manager.h>
#include <bnp/components/graphics.h>

namespace  bnp {

	void SpriteAnimationManager::update(entt::registry& registry, float dt) {
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
