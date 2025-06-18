#include <bnp/components/world.h>
#include <bnp/helpers/filesystem_helper.h>
#include <bnp/serializers/world.hpp>
#include <bnp/game/managers/game_manager.h>
#include <bnp/game/components/matter.h>
#include <bnp/game/components/recipes.h>
#include <bnp/game/components/factions.h>
#include <bnp/game/components/ships.h>
#include <bnp/game/components/universe.h>
#include <bnp/game/components/inventory.h>
#include <bnp/game/serializers/matter.hpp>
#include <bnp/game/serializers/factions.hpp>
#include <bnp/game/serializers/recipes.hpp>
#include <bnp/game/serializers/ships.hpp>
#include <bnp/game/serializers/universe.hpp>
#include <bnp/game/serializers/inventory.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <filesystem>
#include <fstream>

namespace bnp {
namespace Game {
namespace Manager {

void GameManager::initialize(entt::registry& registry) {
	load_static_data(registry);
}

void GameManager::load_static_data(entt::registry& registry) {
	Marshaling::load_component_set<
		ComponentSet<Component::Faction>
	>(registry, data_dir() / "factions.bin");

	Marshaling::load_component_set<
		ComponentSet<Component::Chemical>
	>(registry, data_dir() / "chemicals.bin");

	Marshaling::load_component_set<
		ComponentSet<Component::Universe>,
		ComponentSet<Component::Galaxy>,
		ComponentSet<Component::System>,
		ComponentSet<Component::Celestial, Planet2D>
	>(registry, data_dir() / "universe.bin");

	Marshaling::load_component_set<
		ComponentSet<Component::ShipBlueprint>,
		ComponentSet<Component::ShipSegment>,
		ComponentSet<Component::EngineBlueprint>,
		ComponentSet<Component::FluidStorageBlueprint>
	>(registry, data_dir() / "blueprints.bin");

	Marshaling::load_component_set<
		ComponentSet<Component::Item>
	>(registry, data_dir() / "items.bin");

	Marshaling::load_component_set<
		ComponentSet<Component::ChemicalRecipe>
	>(registry, data_dir() / "recipes.bin");
}

void GameManager::create_save_data(entt::registry& registry) {
	// figure out file name
	// serialize
}

void GameManager::load_save_data(entt::registry& registry) {
	// if already loaded, call `unload_save_data`
	// deserialize
}

void GameManager::unload_save_data(entt::registry& registry) {
	// set loaded=false
	// destroy all entities with `SaveData` component
}

}
}
}
