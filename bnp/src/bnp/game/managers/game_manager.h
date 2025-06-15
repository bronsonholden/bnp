// Game manager is responsible for loading static data

#pragma once

#include <bnp/serializers/registry.hpp>
#include <bnp/game/components/matter.h>
#include <bnp/game/components/recipes.h>
#include <bnp/game/components/ships.h>
#include <bnp/game/components/universe.h>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Manager {

class GameManager {
public:
	GameManager() = default;

	void initialize(entt::registry& registry);

private:
	void load_chemicals(entt::registry& registry);
	void load_universe(entt::registry& registry);
	void load_blueprints(entt::registry& registry);
	void load_recipes(entt::registry& registry);
};

}
}
}
