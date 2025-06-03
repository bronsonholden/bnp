#pragma once

#include <bnp/game/components/universe.h>

#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>

namespace bnp {
namespace Game {
namespace Component {

template <typename S>
void serialize(S& s, Game::Component::System& system) {
	s.value4b(system.id);
	s.text1b(system.name, 256);
	s.value4b(system.galaxy_position.x);
	s.value4b(system.galaxy_position.y);
	s.value8b(system.light_minute_scale);
}

template <typename S>
void serialize(S& s, Game::Component::Celestial& celestial) {
	s.value4b(celestial.id);
	s.text1b(celestial.name, 256);
	s.value8b(celestial.orbit_radius);
	s.value8b(celestial.initial_orbit_progression);
	s.value8b(celestial.orbit_progression);
	s.value8b(celestial.orbit_duration);
	s.value8b(celestial.initial_rotate_progression);
	s.value8b(celestial.rotate_progression);
	s.value8b(celestial.rotate_duration);
}

}
}
}
