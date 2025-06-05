#pragma once

#include <bnp/game/components/inventory.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>

namespace bnp {
namespace Game {
namespace Component {

template <typename S>
void serialize(S& s, Item& item) {
	s.value4b(item.id);
	s.text1b(item.name, 256);
}

}
}
}
