#pragma once

#include <bnp/components/world.h>
#include <bnp/serializers/glm.hpp>

#include <bitsery/bitsery.h>

namespace bnp {

template <typename S>
void serialize(S& s, Planet2D& planet) {
	// planet.rotation is dynamic, determined by universe model
	s.value4b(planet.noise_radius);
	s.value4b(planet.noise_seed);
	s.object(planet.axis);
	s.object(planet.sun_direction);
	s.value4b(planet.water_depth);
	s.value4b(planet.coast_depth);
	s.value4b(planet.mainland_depth);
	s.object(planet.water_color);
	s.object(planet.coast_color);
	s.object(planet.mainland_color);
	s.object(planet.mountain_color);
	s.object(planet.cloud_color);
	s.object(planet.ice_cap_color);
	s.object(planet.crater_color);
	s.object(planet.crater_rim_color);
	s.value4b(planet.cloud_depth);
	s.value4b(planet.cloud_radius);
	s.value4b(planet.cloud_radius_equator_exp);
	s.value4b(planet.cloud_banding_equator_exp);
	s.value4b(planet.crater_step);
	s.value4b(planet.num_craters);
	s.value4b(planet.ice_cap_min);
	s.value4b(planet.ice_cap_max);
}

}
