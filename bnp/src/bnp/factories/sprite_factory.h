#pragma once

#include <bnp/core/node.hpp>
#include <bnp/components/graphics.h>

#include <fstream>
#include <nlohmann/json.hpp>

namespace bnp {

class SpriteFactory {
public:
	SpriteFactory() = default;

	void load_from_aseprite(Node& node, const std::filesystem::path& json_path);
	glm::ivec4 load_body_slice(const nlohmann::json& meta);
};

} // namespace bnp
