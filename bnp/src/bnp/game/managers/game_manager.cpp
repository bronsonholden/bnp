#include <bnp/components/world.h>
#include <bnp/helpers/filesystem_helper.h>
#include <bnp/serializers/world.hpp>
#include <bnp/game/managers/game_manager.h>
#include <bnp/game/serializers/matter.hpp>
#include <bnp/game/serializers/recipes.hpp>
#include <bnp/game/serializers/ships.hpp>
#include <bnp/game/serializers/universe.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <filesystem>
#include <fstream>

namespace bnp {
namespace Game {
namespace Manager {

void GameManager::initialize(entt::registry& registry) {
	load_chemicals(registry);
	load_universe(registry);
	load_blueprints(registry);
	load_recipes(registry);
}

void GameManager::load_chemicals(entt::registry& registry) {
	std::filesystem::path file_path = data_dir() / "chemicals.bin";
	std::ifstream is(file_path, std::ios::binary);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	bnp::deserialize<decltype(des), Component::Chemical>(des, registry);
}

void GameManager::load_universe(entt::registry& registry) {
	std::filesystem::path file_path = data_dir() / "universe.bin";
	std::ifstream is(file_path, std::ios::binary);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	// universe
	{
		bnp::deserialize<decltype(des), Game::Component::Universe>(des, registry);
	}

	// galaxies
	{
		bnp::deserialize<decltype(des), Game::Component::Galaxy>(des, registry);
	}

	// systems
	{
		bnp::deserialize<decltype(des), Game::Component::System>(des, registry);
	}

	// celestials
	{
		bnp::deserialize<decltype(des), Game::Component::Celestial, Planet2D>(des, registry);
	}
}

void GameManager::load_blueprints(entt::registry& registry) {
	std::filesystem::path file_path = data_dir() / "blueprints.bin";
	std::ifstream is(file_path, std::ios::binary);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	if (!is.is_open()) return;

	// ship blueprints
	{
		bnp::deserialize<decltype(des), Component::ShipBlueprint>(des, registry);
	}

	// ship segments
	{
		bnp::deserialize<decltype(des), Component::ShipSegment>(des, registry);
	}

	// engines
	{
		bnp::deserialize<decltype(des), Component::EngineBlueprint>(des, registry);
	}

	// fluid storage
	{
		bnp::deserialize<decltype(des), Component::FluidStorageBlueprint>(des, registry);
	}
}

void GameManager::load_recipes(entt::registry& registry) {
	std::filesystem::path file_path = data_dir() / "recipes.bin";
	std::ifstream is(file_path, std::ios::binary);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	if (!is.is_open()) return;

	// chemical recipes
	{
		bnp::deserialize<decltype(des), Component::ChemicalRecipe>(des, registry);
	}
}

}
}
}
