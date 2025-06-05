#pragma once

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

struct ItemDatabase {
	std::unordered_map<Item::ID, Item> entries;
};

}
}
}
