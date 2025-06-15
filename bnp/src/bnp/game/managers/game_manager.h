// Game manager is responsible for loading static data

#pragma once

#include <bnp/marshaling.hpp>

namespace bnp {
namespace Game {
namespace Manager {

// todo: this can probably be moved out of game-specific code to engine code
class GameManager {
public:
	GameManager() = default;

	void initialize(entt::registry& registry);
};

}
}
}
