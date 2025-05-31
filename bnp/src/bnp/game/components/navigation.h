// components for navigation through the galaxy via the galaxy and system maps

#pragma once

#include <bnp/game/components/universe.h>

namespace bnp {
namespace Game {
namespace Component {

struct GalaxyMap {
	char placeholder = 0;
};

struct SystemMap {
	System::ID system_id;
};

// component for systems displayed on the galaxy map
struct GalaxyMapSystem {
	System::ID system_id;
};

// component for celestials displayed on the system map
struct SystemMapCelestial {
	System::ID system_id;
	int celestial_id;
};

}
}
}
