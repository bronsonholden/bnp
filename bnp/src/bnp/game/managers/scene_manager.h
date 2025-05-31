#pragma once

#include <bnp/game/components/universe.h>

#include <entt/entt.hpp>

namespace bnp {

// manages which entities are visible via the `Renderable` tag
class SceneManager {
public:
	SceneManager() = default;

	void update(entt::registry& registry, float dt);

	void show_galaxy_map(entt::registry& registry);
	void hide_galaxy_map(entt::registry& registry);
	void show_system_map(entt::registry& registry, Game::Component::System::ID system_id);
	void hide_system_map(entt::registry& registry);
};

}
