#pragma once

#include <bnp/game/components/recipes.h>

#include <string>
#include <vector>

namespace bnp {
namespace Game {
namespace Component {

//////////////////////////////////////////////////////////////////////////
// Blueprints are just designs for ships and modules
//////////////////////////////////////////////////////////////////////////

struct ShipSegment {
	static const uint32_t latest_version = 1;
	typedef uint32_t ID;

	uint32_t version = 1;
	ID id;
	std::string name;
	uint32_t engine_slots;
	uint32_t reactor_slots;
	uint32_t solid_storage_slots;
	uint32_t fluid_storage_slots;
};

struct ShipBlueprint {
	static const uint32_t latest_version = 1;
	typedef uint32_t ID;

	uint32_t version = 1;
	ID id;
	std::string name = "R-9 Courier";
	std::string manufacturer = "Tulman-Droupe Stellarworks";

	// mass of just the ship with no modules
	double mass;

	std::vector<ShipSegment> segments;
};

struct EngineBlueprint {
	typedef uint32_t ID;

	ID id;
	std::vector<ChemicalRecipe::ID> propulsion_recipes;
	double mass;
	double effiency_factor;
};

struct FluidStorageBlueprint {
	typedef uint32_t ID;

	ID id;
	double mass;
	double max_volume;
	double max_pressure;
};

//////////////////////////////////////////////////////////////////////////
// Components for actual ships existing in game world. Emplacements
// refer to actual built/purchased modules attached to actual ships in the
// game world.
//////////////////////////////////////////////////////////////////////////

struct Ship {
	typedef uint32_t ID;

	ID id;
	ShipBlueprint::ID ship_blueprint_id;
	// organization ID, 1 = player
	uint32_t owner;
};

// Base emplacement data. Specific modules will also have the
// corresponding component struct (e.g. fluid storage modules
// will have `FluidStorageEmplacement`.)
struct ShipModuleEmplacement {
	typedef uint32_t ID;

	// which instantiated ship this module is attached to
	Ship::ID ship_id;
	// which segment of ship the module occupies
	ShipSegment::ID segment_id;
	// which slot the module occupies
	uint32_t slot_index;
	ID id;
	// mass of the module, total ship mass is ship + all modules
	double mass;
};

struct FluidStorageEmplacement {
	FluidStorageBlueprint::ID fluid_storage_blueprint_id;
	Chemical::ID assigned_chemical_id;
};

struct EngineEmplacement {
	EngineBlueprint::ID engine_blueprint_id;
	ChemicalRecipe::ID assigned_propulsion_recipe;
};

}
}
}
