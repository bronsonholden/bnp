#include <bnp/managers/motility_manager.h>

namespace bnp {

	void MotilityManager::update(entt::registry& registry, float dt) {
		auto view = registry.view<Motility, PhysicsBody2D>();

		for (auto entity : view) {
			auto& motility = registry.get<Motility>(entity);
			auto& body = registry.get<PhysicsBody2D>(entity);

			// === FLYING HANDLING ===
			if (motility.flying) {
				motility.current_velocity = glm::mix(motility.current_velocity, motility.impulse, motility.flying_response);
				glm::ivec2 invalid = glm::isnan(motility.current_velocity);

				if (glm::length(motility.current_velocity) < 0.001f || invalid.x || invalid.y) {
					body.body->SetLinearVelocity({ 0, 0 });
					motility.current_velocity = glm::vec3(0);
				}
				else {
					body.body->SetLinearVelocity({
						motility.current_velocity.x * motility.speed,
						motility.current_velocity.y * motility.speed
						});
				}

				body.body->SetGravityScale(0.0f);
				continue;
			}

			// === ON-GROUND MOVEMENT ===
			b2Vec2 velocity = body.body->GetLinearVelocity();

			// === JUMP START ===
			if (motility.start_jump && !motility.jumping && !motility.falling) {
				body.body->ApplyLinearImpulseToCenter({ 0, motility.jump_height }, true);
				motility.start_jump = false;
				motility.jumping = true;

				// Refresh velocity to reflect applied impulse
				velocity.y = body.body->GetLinearVelocity().y;
			}

			// === SHORT JUMP CUT ===
			if (motility.stop_jump && motility.jumping) {
				velocity.y *= 0.5f;  // jump cut
				motility.stop_jump = false;
			}

			// === TRANSITION TO FALLING ===
			if (velocity.y <= 0.0f) {
				motility.jumping = false;
				motility.start_jump = false;
				motility.falling = true;
				motility.stop_jump = false;
			}

			// === TRANSITION FROM FALLING TO GROUNDED ===
			if (motility.falling && std::abs(velocity.y) < 0.01f) {
				motility.falling = false;
				motility.fall_time = 0.0f;
			}
			else if (motility.falling) {
				motility.fall_time += dt;
			}

			// === WALK / IDLE STATES ===
			bool has_input = std::abs(motility.impulse.x) > 0.001f;

			motility.walking = has_input;
			motility.idle = !has_input;

			// === HORIZONTAL MOVEMENT ===
			if (has_input) {
				velocity.x = motility.impulse.x * motility.speed;
			}
			else {
				// Rapid deceleration
				velocity.x = glm::mix(velocity.x, 0.0f, 0.5f);
			}

			// === UPDATE VELOCITY ===
			motility.current_velocity = glm::vec3(velocity.x, velocity.y, 0.0f);
			body.body->SetLinearVelocity(velocity);
			body.body->SetGravityScale(1.0f);
		}
	}

}
