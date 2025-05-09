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

		glm::vec3 impulse(0.0f);

		if (keyboard[SDL_SCANCODE_A]) {
			impulse.x = -1;
		}

		if (keyboard[SDL_SCANCODE_D]) {
			impulse.x = 1;
		}

		auto& body = registry_.get<PhysicsBody2D>(entity_);

		if (glm::length(impulse) > 0.001f) {
			impulse = glm::normalize(impulse);
		}

		// todo: move to physics manager
		//body.body->SetLinearVelocity(b2Vec2{ new_velocity_x, 0 });

		auto& updated_motility = registry_.patch<Motility>(entity_, [=](Motility& m) {
			m.impulse = impulse;

			if (event.type == SDL_KEYDOWN) {
				if (!m.jumping && !m.falling && event.key.keysym.sym == SDLK_SPACE && event.key.repeat == 0) {
					m.start_jump = true;
					m.stop_jump = false;
				}
			}

			if (event.type == SDL_KEYUP) {
				if (m.jumping) {
					m.stop_jump = true;
				}
			}

			if (m.idle && glm::length(impulse) > 0.001f) {
				m.walking = true;
				m.idle = false;
			}
			else if (m.walking && glm::length(impulse) < 0.001f) {
				m.walking = false;
				m.idle = true;
			}

			if (m.idle && keyboard[SDL_SCANCODE_S]) {
				m.idle = false;
				m.crouching = true;
			}

			if (m.crouching && !keyboard[SDL_SCANCODE_S]) {
				m.idle = true;
				m.crouching = false;
			}
			});
	}

	glm::vec2 Controller::input_direction() const {
		return input_direction_;
	}

}
