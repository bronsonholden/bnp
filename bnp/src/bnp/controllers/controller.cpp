#include <bnp/controllers/controller.h>

namespace bnp {


	Controller::Controller(entt::registry& registry, entt::entity entity)
		: registry_(registry), entity_(entity) {
		if (!registry_.all_of<Motility, Controllable>(entity_)) {
			throw std::runtime_error("Entity does not have required components: Motility and Controllable.");
		}
	}

	void Controller::process_event(const SDL_Event& event) {
		const Uint8* keyboard = SDL_GetKeyboardState(nullptr);

		auto& motility = registry_.get<Motility>(entity_);

		float speed = 0;

		if (keyboard[SDL_SCANCODE_A]) {
			speed -= motility.speed;
		}

		if (keyboard[SDL_SCANCODE_D]) {
			speed += motility.speed;
		}

		input_direction_.x = speed;

		auto& body = registry_.get<PhysicsBody2D>(entity_);
		float new_velocity_x = input_direction_.x;

		body.body->SetLinearVelocity(b2Vec2{ new_velocity_x, 0 });

		registry_.patch<Motility>(entity_, [=](Motility& m) {
			m.current_velocity = glm::vec3{ new_velocity_x, 0, 0 };

			if (m.idle && glm::length(m.current_velocity) > 0.001f) {
				m.walking = true;
				m.idle = false;
			}

			if (m.walking && glm::length(m.current_velocity) < 0.001f) {
				m.walking = false;
				m.idle = true;
			}
			});
	}

	glm::vec2 Controller::input_direction() const {
		return input_direction_;
	}

}
