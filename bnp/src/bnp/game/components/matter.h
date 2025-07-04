#pragma once

#include <string>

namespace bnp {
namespace Game {
namespace Component {

// for raw resources
struct Chemical {
	const static uint32_t latest_version = 1;
	typedef uint32_t ID;

	uint32_t version = latest_version;
	ID id;
	std::string name = "Water";
	std::string formula = "H2O";
	double molecular_mass = 18.01528; // g/mol
	// at 1 atm (101.325 kPa)
	double melting_point = 273.15;
	double boiling_point = 373.15;
	// at melting point
	double solid_density = 917.0; // kg/m^3
	double liquid_density = 1000.0; // kg/m^3
	double specific_heat_gas = 1996.0; // J/(kg K)
};

// complex objects e.g. steel bars, heat shields
struct Material {
	double mass; // kg
};

}
}
}
