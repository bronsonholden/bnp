#include <bnp/components/graphics.h>
#include <bnp/game/managers/scene_manager.h>
#include <bnp/game/components/universe.h>
#include <bnp/game/components/maps.h>

namespace bnp {

void SceneManager::galaxy_map(entt::registry& registry, bool shown) {
	{
		auto view = registry.view<GalaxyMap>();

		for (auto entity : view) {
			registry.emplace_or_replace<Renderable>(entity, shown);
		}
	}

	{
		auto view = registry.view<GalaxyMapSystem>();

		for (auto entity : view) {
			registry.emplace_or_replace<Renderable>(entity, shown);
		}
	}
}

}
