#pragma once

#include <string>
#include <unordered_set>

namespace bnp {

	struct Label {
		std::string value;
	};

	struct Tags {
		std::unordered_set<std::string> values;
	};

}
