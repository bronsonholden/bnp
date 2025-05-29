#include <bnp/managers/game/scene_manager.h>
#include <bnp/components/graphics.h>
#include <bnp/components/game/universe.h>

namespace bnp {

	void SceneManager::galaxy_map(entt::registry& registry, bool shown) {
		auto view = registry.view<GalaxyMapTag>();

		for (auto entity : view) {
			registry.emplace_or_replace<Renderable>(entity, shown);
		}
	}

}
