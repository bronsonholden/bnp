#pragma once

#include <bnp/core/node.hpp>
#include <bnp/components/graphics.h>

#include <fstream>
#include <nlohmann/json.hpp>

namespace bnp {

	class SpriteFactory {
	public:
		SpriteFactory() = default;

		void load_from_aseprite(Node& node, const std::string& json_path);
	};

} // namespace bnp
