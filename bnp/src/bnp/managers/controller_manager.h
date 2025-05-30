#pragma once

#include <bnp/components/controllable.h>

#include <SDL.h>
#include <entt/entt.hpp>

namespace bnp {

class ControllerManager {
public:
	ControllerManager() = default;

	void update(entt::registry& registry, float dt);
	void process_event(entt::registry& registry, const SDL_Event& event);
	void update_keyboard_input_observers(entt::registry& registry);
};

}
