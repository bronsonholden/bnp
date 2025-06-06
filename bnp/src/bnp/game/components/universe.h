#pragma once

#include <glm/glm.hpp>
#include <bitsery/bitsery.h>
#include <unordered_map>
#include <string>
#include <numbers>

namespace bnp {
namespace Game {
namespace Component {

// 1 second in game == 1 hour simulation time
constexpr double UNIVERSAL_TIME_SCALE = 3600.0;

// we store masses in trillions kg so scale G (6.6743e-11) by 10e12
constexpr double G = 6.6743e1;
// Pascals (Pa) to pounds per square inch (PSI)
constexpr double PA_PER_PSI = 6894.76;
constexpr double R = 8.314;

struct Universe {
	// real-time elapsed
	double time_elapsed = 0;
};

struct Galaxy {
	const static uint32_t latest_version = 1;

	uint32_t version = latest_version;
	// per worldspace unit, used to calculate distances between systems
	double light_year_scale;
};

struct System {
	const static uint32_t latest_version = 1;
	typedef int ID;

	uint32_t version = latest_version;
	ID id;
	std::string name;
	// worldspace position, since systems are static within galaxy
	glm::vec2 galaxy_position;
	// per world unit, used to calculate distances between celestials
	double light_minute_scale;
};

struct Celestial {
	const static uint32_t latest_version = 2;
	typedef int ID;

	uint32_t version = latest_version;
	ID id;
	System::ID system_id;
	std::string name;
	// worldspace positioning within system, since celestials will orbit within
	// system as game progresses
	double orbit_radius; // worldspace
	double initial_orbit_progression; // radians
	double orbit_progression; // radians
	// real-time to complete orbit
	double orbit_duration; // seconds
	double initial_rotate_progression; // radians
	double rotate_progression; // radians
	// real-time to complete rotation on axis
	double rotate_duration; // seconds

	// default values are Earth-like
	double mass = 5.23366837e12; // trillions kg
	double radius = 6.12773e6; // meters

	double g() {
		return (G * mass) / (radius * radius);
	}
};

struct Atmosphere {
	//double column_mass() {
	//	return atmospheric_mass / (4.0 * std::numbers::pi * radius * radius);
	//}

	//double atmospheric_pressure() {
	//	return (g() * column_mass()) / PA_PER_PSI;
	//}
};

}
}
}
