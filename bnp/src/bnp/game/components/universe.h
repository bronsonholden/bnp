#pragma once

#include <string>

namespace bnp {
namespace Game {
namespace Component {

struct System {
	typedef int ID;

	ID id;
	std::string name;
	// worldspace position, since systems are static within galaxy
	glm::vec2 galaxy_position;
	// per world unit, used to calculate distances between celestials
	double light_minute_scale;
};

struct Celestial {
	typedef int ID;

	ID id;
	System::ID system_id;
	std::string name;
	// worldspace positioning within system, since celestials will orbit within
	// system as game progresses
	double orbit_radius;
	double orbit_progression;
	// time to complete orbit in seconds
	double orbit_speed;
};

struct Galaxy {
	// per worldspace unit, used to calculate distances between systems
	double light_year_scale;
};

}
}
}
