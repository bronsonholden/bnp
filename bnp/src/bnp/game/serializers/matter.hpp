#pragma once

#include <bnp/game/components/matter.h>

#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

namespace bnp {
namespace Game {
namespace Component {

template <typename S>
void serialize(S& s, Game::Component::Chemical& chemical) {
	s.value4b(chemical.version);
	s.value4b(chemical.id);
	s.text1b(chemical.name, 256);
	s.text1b(chemical.formula, 256);
	s.value8b(chemical.molecular_mass);
	s.value8b(chemical.melting_point);
	s.value8b(chemical.boiling_point);
	s.value8b(chemical.solid_density);
	s.value8b(chemical.liquid_density);
	s.value8b(chemical.specific_heat_gas);
}

}
}
}
