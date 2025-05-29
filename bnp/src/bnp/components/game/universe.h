#pragma once

#include <string>

namespace bnp {

	struct Celestial {
		std::string name;
	};

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

}
