#pragma once

#include <glm/glm.hpp>
#include <string>
#include <stdexcept>

namespace bnp {

namespace ColorHelper {

glm::vec4 hex_to_rgba(const std::string& hex, float alpha = 1.0f) {
	std::string clean_hex = hex;

	if (hex[0] == '#') {
		clean_hex = hex.substr(1);
	}

	if (clean_hex.length() != 6) {
		throw std::invalid_argument("Hex color must be 6 characters (RRGGBB).");
	}

	int r = std::stoi(clean_hex.substr(0, 2), nullptr, 16);
	int g = std::stoi(clean_hex.substr(2, 2), nullptr, 16);
	int b = std::stoi(clean_hex.substr(4, 2), nullptr, 16);

	return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, alpha);
}

}

}
