#pragma once

#include <bnp/core/logger.hpp>
#include <bnp/game/components/universe.h>

#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>

namespace bnp {
namespace Game {
namespace Component {

template <typename S>
void serialize(S& s, Game::Component::Galaxy& galaxy) {
	s.value4b(galaxy.version);
	s.value8b(galaxy.light_year_scale);
}

template <typename S>
void serialize(S& s, Game::Component::System& system) {
	s.value4b(system.version);
	s.value4b(system.id);
	s.text1b(system.name, 256);
	s.value4b(system.galaxy_position.x);
	s.value4b(system.galaxy_position.y);
	s.value8b(system.light_minute_scale);
}

template <typename S>
void serialize(S& s, Game::Component::Celestial& celestial) {
	s.value4b(celestial.version);
	s.value4b(celestial.id);
	s.value4b(celestial.system_id);
	s.text1b(celestial.name, 256);
	s.value8b(celestial.orbit_radius);
	s.value8b(celestial.initial_orbit_progression);
	s.value8b(celestial.orbit_progression);
	s.value8b(celestial.orbit_duration);
	s.value8b(celestial.initial_rotate_progression);
	s.value8b(celestial.rotate_progression);
	s.value8b(celestial.rotate_duration);
	s.value8b(celestial.mass);
	s.value8b(celestial.radius);
	Log::info("-----------------------------------------");
	Log::info("version: %d", celestial.version);
	Log::info("id: %d", celestial.id);
	Log::info("name: %s", celestial.name.c_str());
	Log::info("orbit_radius: %2.5f", celestial.orbit_radius);
	Log::info("initial_orbit_progression: %2.5f", celestial.initial_orbit_progression);
	Log::info("orbit_progression: %2.5f", celestial.orbit_progression);
	Log::info("orbit_duration: %2.5f", celestial.orbit_duration);
	Log::info("initial_rotate_progression: %2.5f", celestial.initial_rotate_progression);
	Log::info("rotate_progression: %2.5f", celestial.rotate_progression);
	Log::info("rotate_duration: %2.5f", celestial.rotate_duration);
	Log::info("-----------------------------------------");
}

}
}
}
