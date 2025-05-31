#pragma once

#include <bnp/game/components/universe.h>

namespace bnp {
namespace Game {
namespace Component {

struct CapitalShip {
	// where the capital ship is
	System::ID system_id;
	Celestial::ID celestial_id;
};

}
}
}
