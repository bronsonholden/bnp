#include <bnp/managers/controller_manager.h>

namespace bnp {

	void ControllerManager::update(entt::registry& registry, float dt) {
		auto view = registry.view<Platform2DController>();

		for (auto entity : view) {
			auto& controller = view.get<Platform2DController>(entity);
			controller.update(registry, entity, dt);
		}
	}

	void ControllerManager::process_event(entt::registry& registry, const SDL_Event& event) {
	}

	void ControllerManager::update_keyboard_input_observers(entt::registry& registry) {
		auto view = registry.view<KeyboardInputObserver, KeyboardInputMapping>();
		const Uint8* keyboard = SDL_GetKeyboardState(nullptr);

		for (auto entity : view) {
			auto& mapping = view.get<KeyboardInputMapping>(entity);

			auto& observer = view.get<KeyboardInputObserver>(entity);

			observer.inputs.clear();

			for (auto& [scancode, action] : mapping.values) {
				observer.inputs.emplace(
					action,
					keyboard[scancode] ?
					KeyboardInput::State::Pressed :
					KeyboardInput::State::Released
				);
			}
		}
	}

}
