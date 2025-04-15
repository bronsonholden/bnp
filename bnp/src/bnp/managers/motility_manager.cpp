#include <bnp/managers/motility_manager.h>

namespace bnp {

	void MotilityManager::update(entt::registry& registry, float dt) {
		auto view = registry.view<Motility, PhysicsBody2D>();

		for (auto entity : view) {
			auto& motility = view.get<Motility>(entity);
			auto& body = view.get<PhysicsBody2D>(entity);
			glm::vec3 velocity = motility.impulse * motility.speed;

			auto& updated_motility = registry.patch<Motility>(entity, [=](Motility& m) {
				if (m.idle) {
					if (glm::length(motility.impulse) > 0.001f) {
						m.walking = true;
						m.idle = false;
					}
				}

				if (m.walking) {
					m.current_velocity = velocity;

					if (glm::length(motility.impulse) < 0.001f) {
						m.walking = false;
						m.idle = true;
					}
				}
				else {
					m.current_velocity = glm::vec3(0);
				}
				});


			body.body->SetLinearVelocity(b2Vec2{ updated_motility.current_velocity.x, updated_motility.current_velocity.y });
		}
	}

}
