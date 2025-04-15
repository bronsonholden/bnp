#include <bnp/managers/motility_manager.h>

namespace bnp {

	void MotilityManager::update(entt::registry& registry, float dt) {
		auto view = registry.view<Motility, PhysicsBody2D>();

		for (auto entity : view) {
			auto& old_motility = view.get<Motility>(entity);
			auto& body = view.get<PhysicsBody2D>(entity);

			auto& updated_motility = registry.patch<Motility>(entity, [&](Motility& m) {
				if (m.start_jump) {
					body.body->ApplyLinearImpulseToCenter(b2Vec2(0, m.jump_height), true);
					m.start_jump = false;
					m.jumping = true;
				}

				b2Vec2 body_velocity = body.body->GetLinearVelocity();

				m.current_velocity = glm::vec3{ body_velocity.x, body_velocity.y, 0 };

				if (m.jumping) {
					if (body_velocity.y < 0) {
						m.jumping = false;
						m.falling = true;
					}
				}

				if (m.falling) {
					if (std::abs(body_velocity.y) < 0.001f) {
						m.falling = false;
						m.fall_time = 0;
						m.air_control = false;
					}
					else {
						m.fall_time += dt;
					}
				}

				if (m.idle) {
					if (std::abs(m.impulse.x) > 0.001f) {
						m.walking = true;
						m.idle = false;
					}
				}

				if (m.walking) {
					if (std::abs(m.impulse.x) < 0.001f) {
						m.walking = false;
						m.idle = true;
					}
				}

				if (m.air_control) {
					m.current_air_control = std::max(m.current_air_control * m.air_control_ramp, 0.4f);
				}
				else {
					m.current_air_control = 1.0f;
				}

				if (m.walking) {
					/*if ((m.jumping || m.falling) && std::abs(body_velocity.x) > 0.0f && (m.impulse.x * body_velocity.x) < 0.0f) {
						m.current_velocity.x = body_velocity.x * m.air_control;
					}*/

					if (!m.air_control) {
						m.current_velocity.x = m.impulse.x * m.speed;
					}
					else {
						m.current_velocity.x = m.impulse.x * m.speed * m.current_air_control;
					}

					if ((m.jumping || m.falling) && (std::abs(body_velocity.x) < 0.001f || (std::abs(body_velocity.x) > 0.0f && (body_velocity.x * m.impulse.x) < 0.0f))) {
						m.current_velocity.x = m.impulse.x * m.speed;
						m.air_control = true;
					}
				}
				else {
					if (m.jumping) {
						m.current_velocity.x = body_velocity.x * m.air_control_ramp;
					}
					else if (m.falling) {
						m.current_velocity.x = body_velocity.x * m.air_control_ramp;
					}
					else {
						m.current_velocity.x = 0;
					}
				}
				});

			body.body->SetLinearVelocity(b2Vec2{ updated_motility.current_velocity.x, updated_motility.current_velocity.y });
		}
	}

}
