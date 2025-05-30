#pragma once

#include <bnp/components/inventory.h>

#include <vector>

namespace bnp {
namespace Game {
namespace Component {

// stores the resources that can be extracted from a celestial body
// (planet, moon, asteroid)
struct CelestialExtractionTable {
	std::vector<Item::ID> entries;
};

}
}
}
