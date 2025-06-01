#pragma once

#include <bnp/game/components/universe.h>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Manager {

// manages entities related to navigation (galaxy map, system map, etc.)
class NavigationManager {
public:
	NavigationManager() = default;

	void update(entt::registry& registry, float dt);

	void show_galaxy_map(entt::registry& registry);
	void hide_galaxy_map(entt::registry& registry);
	void show_system_map(entt::registry& registry, Game::Component::System::ID system_id);
	void hide_system_map(entt::registry& registry);
	void show_celestial_map(entt::registry& registry, Game::Component::Celestial::ID celestial_id);
	void hide_celestial_map(entt::registry& registry);

	// update the sprite layer of galaxy/system map dots (visited, unvisited, present)
	void update_map_dot_layers(entt::registry& registry);
};

}
}
}
