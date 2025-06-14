#pragma once

#include <bnp/core/logger.hpp>
#include <bnp/game/components/universe.h>
#include <bnp/game/serializers/matter.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>


// serializer for atmosphere composition entries
namespace std {
template <typename S>
void serialize(S& s, pair<double, bnp::Game::Component::Chemical::ID>& entry) {
	s.value8b(entry.first);
	s.value4b(entry.second);
}
}

namespace bnp {
namespace Game {
namespace Component {

template <typename S>
void serialize(S& s, Game::Component::Universe& universe) {
	s.value8b(universe.time_elapsed);
}

template <typename S>
void serialize(S& s, Game::Component::Galaxy& galaxy) {
	s.value4b(galaxy.version);
	s.value8b(galaxy.light_year_scale);

	galaxy.version = galaxy.latest_version;
}

template <typename S>
void serialize(S& s, Game::Component::System& system) {
	s.value4b(system.version);
	s.value4b(system.id);
	s.text1b(system.name, 256);
	s.value4b(system.galaxy_position.x);
	s.value4b(system.galaxy_position.y);
	s.value8b(system.light_minute_scale);

	system.version = system.latest_version;
}

template <typename S>
void serialize(S& s, Game::Component::Celestial& celestial) {
	s.value4b(celestial.version);
	s.value4b(celestial.id);
	s.value4b(celestial.system_id);
	s.text1b(celestial.name, 256);
	s.value8b(celestial.orbit_radius);
	s.value8b(celestial.initial_orbit_progression);
	s.value8b(celestial.orbit_duration);
	s.value8b(celestial.initial_rotate_progression);
	s.value8b(celestial.rotate_duration);
	s.value8b(celestial.mass);
	s.value8b(celestial.radius);
	s.container(celestial.atmosphere, 256, [](S& s, auto& pair) {
		s.value8b(pair.first);
		s.value4b(pair.second);
		});

	celestial.version = celestial.latest_version;
}

}
}
}
