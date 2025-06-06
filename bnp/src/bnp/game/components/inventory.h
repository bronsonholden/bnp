#pragma once

#include <entt/entt.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace bnp {
namespace Game {
namespace Component {

struct Item {
	typedef uint32_t ID;
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
