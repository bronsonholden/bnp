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
	load_component_set<
		ComponentSet<Component::Chemical>
	>(registry, "chemicals.bin");

	load_component_set<
		ComponentSet<Component::Universe>,
		ComponentSet<Component::Galaxy>,
		ComponentSet<Component::System>,
		ComponentSet<Component::Celestial, Planet2D>
	>(registry, "universe.bin");

	load_component_set<
		ComponentSet<Component::ShipBlueprint>,
		ComponentSet<Component::ShipSegment>,
		ComponentSet<Component::EngineBlueprint>,
		ComponentSet<Component::FluidStorageBlueprint>
	>(registry, "blueprints.bin");

	load_component_set<
		ComponentSet<Component::Item>
	>(registry, "items.bin");

	load_component_set<
		ComponentSet<Component::ChemicalRecipe>
	>(registry, "recipes.bin");
}

}
}
}
