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
	typedef uint32_t ID;

	ID id;
	std::string name;
	std::vector<std::pair<double, Chemical::ID>> inputs;
	std::vector<std::pair<double, Chemical::ID>> outputs;
	double energy; // MJ/kg
};

}
}
}
