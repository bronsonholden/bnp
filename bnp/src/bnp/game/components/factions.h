#pragma once

#include <string>

namespace bnp {
namespace Game {
namespace Component {

struct Faction {
	typedef uint32_t ID;

	ID id;
	std::string name;
};

}
}
}
