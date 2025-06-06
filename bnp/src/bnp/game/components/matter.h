#pragma once

namespace bnp {
namespace Game {
namespace Component {

// for raw resources
struct Chemical {
	std::string formula = "H2O";
	double molecular_mass = 18.01528; // g/mol
	// at 1 atm (101.325 kPa)
	double melting_point = 273.15;
	double boiling_point = 373.15;
	// at melting point
	double solid_density = 917.0; // kg/m^3
	double liquid_density = 1000.0; // kg/m^3

	// gamified values here
	double toxicity = 0;
};

// complex objects e.g. steel bars, heat shields
struct Material {
	double mass; // kg
};

}
}
}
