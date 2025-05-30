#include <bnp/managers/sprite_animation_manager.h>
#include <bnp/components/graphics.h>
#include <bnp/components/physics.h>

namespace  bnp {

void SpriteAnimationManager::update(entt::registry& registry, float dt) {
	update_animations(registry, dt);
	tick_animations(registry, dt);
	update_colliders(registry);
}

void SpriteAnimationManager::update_animations(entt::registry& registry, float dt) {
	auto view = registry.view<Sprite, SpriteAnimator, Motility>();

	for (auto entity : view) {
		auto& sprite = registry.get<Sprite>(entity);
		auto& animator = registry.get<SpriteAnimator>(entity);
		auto& motility = registry.get<Motility>(entity);

		registry.patch<Sprite>(entity, [&](Sprite& sprite) {
			if (std::abs(motility.current_velocity.x) > 0.001f) {
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

			if (motility.jumping) {
				new_animation = "Jump";
			}
			else if (motility.falling) {
				new_animation = "Fall";
			}
			else if (motility.flying) {
				new_animation = "Fly";
			}
			else if (motility.walking) {
				new_animation = "Walk";
			}
			else if (motility.idle) {
				new_animation = "Idle";
			}
			else if (motility.crouching) {
				new_animation = "Crouch";
			}

			if (new_animation != animator.current_animation) {
				animator.current_framelist_index = 0;
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

		if (sprite.animations.find(animator.current_animation) == sprite.animations.end()) {
			continue;
		}

		const auto& animation = sprite.animations.at(animator.current_animation);
		const auto& frame = sprite.layers.at(0).frames.at(animation.framelist.at(animator.current_framelist_index));

		size_t new_frame_index = animator.current_framelist_index;
		float new_time = animator.time + dt;
		bool new_done = false;

		if (new_time > frame.duration) {
			new_time -= frame.duration;
			new_frame_index += 1;
		}

		if (new_frame_index >= animation.framelist.size()) {
			if (animation.repeat == 0) {
				new_frame_index = 0;
			}
			else {
				new_done = true;
				new_frame_index = animation.framelist.size() - 1;
			}
		}

		if (new_frame_index != animator.current_framelist_index) {
			auto& new_frame = sprite.layers.at(0).frames.at(new_frame_index);

			// activate colliders for the frame

			//if (registry.all_of<PhysicsBody2D>(entity)) {
			//	auto& body = registry.get<PhysicsBody2D>(entity);

			//	if (frame.collider != new_frame.collider) {
			//		for (b2Fixture* f = body.body->GetFixtureList(); f;) {
			//			b2Fixture* next = f->GetNext();
			//			body.body->DestroyFixture(f);
			//			f = next;
			//		}

			//		float width = static_cast<float>(new_frame.collider.z) / new_frame.size.x;
			//		float height = static_cast<float>(new_frame.collider.w) / new_frame.size.y;

			//		b2Fixture* fixture = body.body->GetFixtureList();
			//		if (!fixture) {
			//			// Create a new polygon shape with the new dimensions
			//			b2PolygonShape shape;
			//			shape.SetAsBox(width / 2, height / 2);

			//			b2FixtureDef fixture_def;
			//			fixture_def.shape = &shape;
			//			fixture_def.density = 1.0f;
			//			fixture_def.friction = 0.3f;
			//			fixture_def.restitution = 0.1f;

			//			body.body->CreateFixture(&fixture_def);
			//		}

			//	}
			//}
		}

		registry.patch<SpriteAnimator>(entity, [&](SpriteAnimator& sa) {
			sa.current_framelist_index = new_frame_index;
			sa.time = new_time;
			sa.done = new_done;
			});
	}
}

void SpriteAnimationManager::update_colliders(entt::registry& registry) {

}

}
