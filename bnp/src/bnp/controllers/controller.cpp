#include <bnp/controllers/controller.h>

namespace bnp {


	Controller::Controller(entt::registry& _registry, entt::entity _entity)
		: registry(_registry), entity(_entity) {
		if (!registry.all_of<Motility, Controllable>(entity)) {
			throw std::runtime_error("Entity does not have required components: Motility and Controllable.");
		}
	}

	void Controller::process_event(const SDL_Event& event) {

		const Uint8* keyboard = SDL_GetKeyboardState(nullptr);

		auto& motility = registry.get<Motility>(entity);

		glm::vec3 impulse(0.0f);

		if (keyboard[SDL_SCANCODE_A]) {
			impulse.x = -1;
		}

		if (keyboard[SDL_SCANCODE_D]) {
			impulse.x = 1;
		}

		auto& body = registry.get<PhysicsBody2D>(entity);

		if (glm::length(impulse) > 0.001f) {
			impulse = glm::normalize(impulse);
		}

		// todo: move to physics manager
		//body.body->SetLinearVelocity(b2Vec2{ new_velocity_x, 0 });

		auto& updated_motility = registry.patch<Motility>(entity, [=](Motility& m) {
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

}
