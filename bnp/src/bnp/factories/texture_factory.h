#pragma once

#include <bnp/components/graphics.h>

#include <filesystem>
#include <fstream>

namespace bnp {

class TextureFactory {
public:
	TextureFactory() = default;

	Texture load_from_file(std::filesystem::path);
};

}
