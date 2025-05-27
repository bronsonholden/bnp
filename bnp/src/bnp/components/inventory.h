#pragma once

#include <string>
#include <vector>

namespace bnp {

	struct Item {
		typedef int ID;
		std::string name;
	};

	struct ItemDatabase {
		std::vector<Item> entries;
	};

}
