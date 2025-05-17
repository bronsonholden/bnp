#include <bnp/components/controllable.h>
#include <bnp/components/physics.h>

namespace bnp {

	void Platform2DController::update(entt::registry& registry, entt::entity entity, float dt) {
		auto& observer = registry.get<KeyboardInputObserver>(entity);
		auto& motility = registry.get<Motility>(entity);
		auto& body = registry.get<PhysicsBody2D>(entity);

		// impulse to set on motility component
		glm::vec3 impulse(0.0f);

		// input states
		bool left = false;
		bool right = false;
		bool down = false;
		bool jump = false;

		// checking keyboard input state
		{
			try {
				if (observer.inputs.at(KeyboardInput::Action::MoveLeft) == KeyboardInput::State::Pressed) {
					left = true;
				}
			}
			catch (const std::out_of_range&) {}

			try {
				if (observer.inputs.at(KeyboardInput::Action::MoveRight) == KeyboardInput::State::Pressed) {
					right = true;
				}
			}
			catch (const std::out_of_range&) {}

			try {
				if (observer.inputs.at(KeyboardInput::Action::MoveDown) == KeyboardInput::State::Pressed) {
					down = true;
				}
			}
			catch (const std::out_of_range&) {}

			try {
				if (observer.inputs.at(KeyboardInput::Action::Jump) == KeyboardInput::State::Pressed) {
					jump = true;
				}
			}
			catch (const std::out_of_range&) {}
		}

		// movement to impulse
		if (left) impulse.x = -1;
		if (right) impulse.x = 1;

		if (glm::length(impulse) > 0.001f) {
			impulse = glm::normalize(impulse);
		}

		auto& updated_motility = registry.patch<Motility>(entity, [=](Motility& m) {
			m.impulse = impulse;

			// todo: maybe this needs to be handled as an event, not observed state
			if (jump) {
				if (!m.jumping && !m.falling && m.can_jump) {
					m.start_jump = true;
					m.stop_jump = false;
					m.can_jump = false;
				}
			}
			else {
				if (m.jumping) {
					m.stop_jump = true;
				}

				if (!m.jumping && !m.falling) {
					m.can_jump = true;
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

			if (m.idle && down) {
				m.idle = false;
				m.crouching = true;
			}

			if (m.crouching && !down) {
				m.idle = true;
				m.crouching = false;
			}
			});
	}
}
