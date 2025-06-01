#include <bnp/core/logger.hpp>
#include <bnp/components/graphics.h>
#include <bnp/components/world.h>
#include <bnp/components/ui.h>
#include <bnp/game/managers/navigation_manager.h>
#include <bnp/game/components/universe.h>
#include <bnp/game/components/navigation.h>
#include <bnp/game/components/fleet.h>
#include <bnp/game/prefabs/celestials.h>

namespace bnp {
namespace Game {
namespace Manager {

void NavigationManager::update(entt::registry& registry, float dt) {
	update_map_dot_layers(registry);

	// update celestial dots along their orbit
	{
		auto view = registry.view<Game::Component::SystemMapCelestial, Transform>();
		auto celestials = registry.view<Game::Component::Celestial>();

		for (auto entity : view) {
			auto& map_celestial = view.get<Game::Component::SystemMapCelestial>(entity);
			Game::Component::Celestial::ID celestial_id = map_celestial.celestial_id;

			// check model component, update transform of dot
			for (auto celestial_entity : celestials) {
				auto& celestial = celestials.get<Game::Component::Celestial>(celestial_entity);

				if (celestial.id == celestial_id) {
					registry.patch<Transform>(entity, [&](Transform& t) {
						t.position = glm::vec3(
							std::cos(celestial.orbit_progression) * celestial.orbit_radius,
							std::sin(celestial.orbit_progression) * celestial.orbit_radius,
							0.0f
						);
						t.dirty = true;
						});
					break;
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

void NavigationManager::update_map_dot_layers(entt::registry& registry) {
	auto capital_ship_view = registry.view<Game::Component::CapitalShip>();
	entt::entity capital_ship_entity = capital_ship_view.front();

	if (capital_ship_entity == entt::null) return;
	auto& capital_ship = capital_ship_view.get<Game::Component::CapitalShip>(capital_ship_entity);

	{
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

	{
		auto view = registry.view<Game::Component::SystemMapCelestial, Sprite>();

		for (auto entity : view) {
			auto& system = view.get<Game::Component::SystemMapCelestial>(entity);
			auto& sprite = view.get<Sprite>(entity);

			if (system.celestial_id == capital_ship.celestial_id) {
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

void NavigationManager::show_system_map(entt::registry& registry, Game::Component::System::ID system_id) {
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
				transform.scale = glm::vec3(celestial.orbit_radius * 2.0f, celestial.orbit_radius * 2.0f, 1.0f);
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
			}

		}
	}
}

void NavigationManager::hide_system_map(entt::registry& registry) {
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

void NavigationManager::show_galaxy_map(entt::registry& registry) {
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

void NavigationManager::hide_galaxy_map(entt::registry& registry) {
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
void NavigationManager::show_celestial_map(entt::registry& registry, Game::Component::Celestial::ID celestial_id) {
	entt::entity celestial_entity = entt::null;

	{
		auto view = registry.view<Game::Component::Celestial, Planet2D>();

		for (auto entity : view) {
			auto& celestial = view.get<Game::Component::Celestial>(entity);

			if (celestial.id == celestial_id) {
				celestial_entity = entity;
				break;
			}
		}
	}

	auto& planet_2d = registry.get<Planet2D>(celestial_entity);

	if (celestial_entity == entt::null) {
		Log::error("No celestial found (clestial_id=%d)", celestial_id);
		return;
	}

	Node celestial = Game::Prefab::Celestials::celestial(registry, *ResourceManager::singleton);
	celestial.add_component<Game::Component::CelestialMap>(celestial_id);
	celestial.add_component<Planet2D>(planet_2d);
}

void NavigationManager::hide_celestial_map(entt::registry& registry) {
	{
		auto view = registry.view<Game::Component::CelestialMap>();

		for (auto entity : view) {
			registry.destroy(entity);
		}
	}
}

}
}
}
