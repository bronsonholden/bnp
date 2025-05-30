#pragma once

#include <entt/entt.hpp>

namespace bnp {

// manages which entities are visible via the `Renderable` tag
class SceneManager {
public:
	SceneManager() = default;

	void update(entt::registry& registry, float dt);
	void galaxy_map(entt::registry& registry, bool shown);
};

}
