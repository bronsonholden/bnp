#pragma once

#include <SDL.h>
#include <entt/entt.hpp>

namespace bnp {

struct Platform2DController {
	bool active = true;
	void update(entt::registry& registry, entt::entity entity, float dt);
};

struct Controllable {
	bool active = true;
};

struct KeyboardInput {
	enum Action {
		MoveLeft = 0,
		MoveRight,
		MoveUp,
		MoveDown,
		Jump
	};

	enum State {
		Pressed = 0,
		Held,
		Released
	};
};

typedef SDL_Scancode KeyScancode;

struct KeyboardInputMapping {
	std::unordered_map<KeyScancode, KeyboardInput::Action> values;
};

struct KeyboardInputObserver {
	std::unordered_map<KeyboardInput::Action, KeyboardInput::State> inputs;
};

}
