#include <bnp/components/world.h>
#include <bnp/helpers/filesystem_helper.h>
#include <bnp/serializers/world.hpp>
#include <bnp/game/managers/game_manager.h>
#include <bnp/game/components/matter.h>
#include <bnp/game/components/recipes.h>
#include <bnp/game/components/ships.h>
#include <bnp/game/components/universe.h>
#include <bnp/game/components/inventory.h>
#include <bnp/game/serializers/matter.hpp>
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
	load_core_data<
		CoreDataSet<Component::Chemical>
	>(registry, "chemicals.bin");

	load_core_data<
		CoreDataSet<Component::Universe>,
		CoreDataSet<Component::Galaxy>,
		CoreDataSet<Component::System>,
		CoreDataSet<Component::Celestial, Planet2D>
	>(registry, "universe.bin");

	load_core_data<
		CoreDataSet<Component::ShipBlueprint>,
		CoreDataSet<Component::ShipSegment>,
		CoreDataSet<Component::EngineBlueprint>,
		CoreDataSet<Component::FluidStorageBlueprint>
	>(registry, "blueprints.bin");

	load_core_data<
		CoreDataSet<Component::Item>
	>(registry, "items.bin");

	load_core_data<
		CoreDataSet<Component::ChemicalRecipe>
	>(registry, "recipes.bin");
}

}
}
}
