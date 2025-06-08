#pragma once

#include <entt/entt.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace bnp {
namespace Game {
namespace Component {

struct Item {
	const static uint32_t latest_version = 1;
	typedef uint32_t ID;

	uint32_t version;
	ID id;
	std::string name;
};

struct Container {
	typedef uint32_t ID;
	ID id;
};

struct ItemIndex {
	std::unordered_map<Item::ID, entt::entity> entries;
};

}
}
}
