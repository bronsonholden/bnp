#include <bnp/components/graphics.h>
#include <bnp/components/ui.h>
#include <bnp/game/managers/scene_manager.h>
#include <bnp/game/components/universe.h>
#include <bnp/game/components/navigation.h>

namespace bnp {

void SceneManager::update(entt::registry& registry, float dt) {
	// clicking on system dot in galaxy map transfers to that system
	{
		auto view = registry.view<Game::Component::GalaxyMapSystem, Button, Renderable>();

		for (auto entity : view) {
			auto& renderable = view.get<Renderable>(entity);
			auto& button = view.get<Button>(entity);
			auto& system = view.get<Game::Component::GalaxyMapSystem>(entity);

			if (button.clicked) {
				registry.patch<Button>(entity, [](Button& b) {
					b.clicked = false;
					});
			}
		}
	}
}

void SceneManager::galaxy_map(entt::registry& registry, bool shown) {
	{
		auto view = registry.view<Game::Component::GalaxyMap>();

		for (auto entity : view) {
			registry.emplace_or_replace<Renderable>(entity, shown);
		}
	}

	{
		auto view = registry.view<Game::Component::GalaxyMapSystem>();

		for (auto entity : view) {
			registry.emplace_or_replace<Renderable>(entity, shown);
		}
	}
}

}
