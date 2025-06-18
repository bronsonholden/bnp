#pragma once

#include <bnp/components/reflection.h>

#include <string>

namespace bnp {
namespace Game {
namespace Component {

struct Faction {
	static const uint32_t latest_version = 1;
	typedef uint32_t ID;

	uint32_t version = latest_version;
	ID id;
	std::string name = "<New faction>";

	REGISTER_COMPONENT_FIELDS(
		std::make_pair("name", &Faction::name)
	);

	REGISTER_COMPONENT_TABLE_HEADER_FIELDS(
		std::make_pair("ID", &Faction::id),
		std::make_pair("Name", &Faction::name)
	);
};

}
}
}
