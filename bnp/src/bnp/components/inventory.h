#pragma once

#include <string>
#include <vector>

namespace bnp {

	struct ItemDatabase {
		std::vector<Item> entries;
	};

	struct Item {
		typedef int ID;
		std::string name;
	};

}
