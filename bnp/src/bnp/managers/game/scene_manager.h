#pragma once

#include <entt/entt.hpp>

namespace bnp {

	// manages which entities are visible via the `Renderable` tag
	class SceneManager {
	public:
		SceneManager() = default;

		void galaxy_map(entt::registry& registry, bool shown);
	};

}
