#pragma once

#include <bnp/game/components/factions.h>

#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>

namespace bnp {
namespace Game {
namespace Component {

template <typename S>
void serialize(S& s, Faction& f) {
	s.value4b(f.id);
	s.text1b(f.name, 256);
}

}
}
}
