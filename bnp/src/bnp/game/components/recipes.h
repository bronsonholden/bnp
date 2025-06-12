#pragma once

#include <bnp/game/components/matter.h>

#include <string>
#include <vector>

namespace bnp {
namespace Game {
namespace Component {

// Chemical recipes take inputs in the prescribed ratio,
// producing outputs similarly in a prescribed ratio.
// The rate of production is determined by the module that
// does the crafting
struct ChemicalRecipe {
	static const uint32_t latest_version = 1;
	typedef uint32_t ID;

	uint32_t version;
	ID id;
	std::string name;
	std::vector<std::pair<double, Chemical::ID>> inputs;
	std::vector<std::pair<double, Chemical::ID>> outputs;
	double energy; // MJ/kg
	double efficiency = 1.0; // [0, 1]; describes loss of mass to energy
	double protons_emitted = 0.0; // count/kg
	double neutrons_emitted = 0.0; // count/kg

};

}
}
}
