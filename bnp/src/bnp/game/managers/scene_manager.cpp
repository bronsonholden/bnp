#include <bnp/core/logger.hpp>
#include <bnp/components/graphics.h>
#include <bnp/components/ui.h>
#include <bnp/game/managers/scene_manager.h>
#include <bnp/game/components/universe.h>
#include <bnp/game/components/navigation.h>
#include <bnp/game/components/fleet.h>
#include <bnp/game/prefabs/celestials.h>

namespace bnp {

void SceneManager::update(entt::registry& registry, float dt) {
	{
		auto capital_ship_view = registry.view<Game::Component::CapitalShip>();
		entt::entity capital_ship_entity = capital_ship_view.front();
		if (capital_ship_entity != entt::null) {
			auto& capital_ship = capital_ship_view.get<Game::Component::CapitalShip>(capital_ship_entity);
			auto view = registry.view<Game::Component::GalaxyMapSystem, Sprite>();

			for (auto entity : view) {
				auto& system = view.get<Game::Component::GalaxyMapSystem>(entity);
				auto& sprite = view.get<Sprite>(entity);

				if (system.system_id == capital_ship.system_id) {
					for (auto& layer : sprite.layers) {
						if (layer.name == "Present") {
							layer.visible = true;
						}
						else {
							layer.visible = false;
						}
					}
				}
			}
		}
	}


	// clicking on system dot in galaxy map transfers to that system
	{
		auto view = registry.view<Game::Component::GalaxyMapSystem, Button, Renderable>();

		for (auto entity : view) {
			auto& renderable = view.get<Renderable>(entity);
			auto& button = view.get<Button>(entity);
			auto& system = view.get<Game::Component::GalaxyMapSystem>(entity);

			if (button.clicked) {
				Log::info("Clicked galaxy map dot (system_id=%d)", system.system_id);

				hide_galaxy_map(registry);
				show_system_map(registry, system.system_id);
			}
		}
	}

	// clicking on a celestial dot in system map transfers to that celestial
	{
		auto view = registry.view<Game::Component::SystemMapCelestial, Button, Renderable>();

		for (auto entity : view) {
			auto& button = view.get<Button>(entity);
			auto& celestial = view.get<Game::Component::SystemMapCelestial>(entity);

			if (button.clicked) {
				Log::info("Clicked system map dot (system_id=%d, celestial_id=%d)", celestial.system_id, celestial.celestial_id);

				hide_system_map(registry);
				show_celestial_map(registry, celestial.celestial_id);
			}
		}
	}
}

void SceneManager::show_system_map(entt::registry& registry, Game::Component::System::ID system_id) {
	auto view = registry.view<Game::Component::System>();
	entt::entity system_entity = entt::null;

	for (auto entity : view) {
		auto& system = view.get<Game::Component::System>(entity);

		if (system.id == system_id) {
			system_entity = entity;
			break;
		}
	}

	if (system_entity == entt::null) {
		Log::error("Unable to find system model (system_id=%d)", system_id);
	}

	{
		auto view = registry.view<Game::Component::Celestial>();

		for (auto entity : view) {
			auto& celestial = view.get<Game::Component::Celestial>(entity);

			if (celestial.system_id != system_id) continue;

			{
				Node orbit = Game::Prefab::Celestials::celestial_orbit(registry, *ResourceManager::singleton);
				orbit.add_component<Game::Component::SystemMap>(system_id);
				auto& transform = orbit.get_component<Transform>();
				transform.position = glm::vec3(0.0f);
				transform.scale = glm::vec3(celestial.orbit_radius, celestial.orbit_radius, 1.0f);
				transform.dirty = true;
			}

			{
				Node dot = Game::Prefab::Celestials::galaxy_map_dot(registry, *ResourceManager::singleton);
				dot.add_component<Game::Component::SystemMapCelestial>(
					Game::Component::SystemMapCelestial{
						.system_id = celestial.system_id,
						.celestial_id = celestial.id
					}
				);
				auto& transform = dot.get_component<Transform>();
				transform.position = glm::vec3(celestial.orbit_radius / 2.0f, 0.0f, 0.0f);
				transform.dirty = true;
			}

		}
	}
}

void SceneManager::hide_system_map(entt::registry& registry) {
	{
		auto view = registry.view<Game::Component::SystemMap>();

		for (auto entity : view) {
			registry.destroy(entity);
		}
	}
	{
		auto view = registry.view<Game::Component::SystemMapCelestial>();

		for (auto entity : view) {
			registry.destroy(entity);
		}
	}
}

void SceneManager::show_galaxy_map(entt::registry& registry) {
	Game::Prefab::Celestials::galaxy(registry, *ResourceManager::singleton);

	{
		auto view = registry.view<Game::Component::System>();

		for (auto entity : view) {
			auto& system = view.get<Game::Component::System>(entity);
			Node dot = Game::Prefab::Celestials::galaxy_map_dot(registry, *ResourceManager::singleton);

			dot.add_component<Game::Component::GalaxyMapSystem>(Game::Component::GalaxyMapSystem{
				system.id
				});

			auto& transform = dot.get_component<Transform>();
			transform.position = glm::vec3(system.galaxy_position, 0);
			transform.dirty = true;
		}
	}
}

void SceneManager::hide_galaxy_map(entt::registry& registry) {
	{
		auto view = registry.view<Game::Component::GalaxyMap>();

		for (auto entity : view) {
			registry.destroy(entity);
		}
	}

	{
		auto view = registry.view<Game::Component::GalaxyMapSystem>();

		for (auto entity : view) {
			registry.destroy(entity);
		}
	}
}

// todo: for celestials with moons, render orbit & dots and a smaller version of the celestial at the center
void SceneManager::show_celestial_map(entt::registry& registry, Game::Component::Celestial::ID celestial_id) {
	Node celestial = Game::Prefab::Celestials::eden(registry, *ResourceManager::singleton);

	celestial.add_component<Game::Component::CelestialMap>(celestial_id);
}

void SceneManager::hide_celestial_map(entt::registry& registry) {
	{
		auto view = registry.view<Game::Component::CelestialMap>();

		for (auto entity : view) {
			registry.destroy(entity);
		}
	}
}

}
