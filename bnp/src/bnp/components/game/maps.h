#pragma once

namespace bnp {

struct GalaxyMapTag {
	char _;
};

struct GalaxyMap {
	char placeholder;
};

// component for systems displayed on the galaxy map
struct GalaxyMapSystem {
	int system_id;
};

struct SystemMap {
	int system_id;
};

// component for celestials displayed on the system map
struct SystemMapCelestial {
	int celestial_id;
};

}
