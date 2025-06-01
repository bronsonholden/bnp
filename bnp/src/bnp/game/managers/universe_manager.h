#pragma once

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Manager {

class UniverseManager {
public:
	UniverseManager() = default;

	void update(entt::registry& registry, float dt);
};

}
}
}
