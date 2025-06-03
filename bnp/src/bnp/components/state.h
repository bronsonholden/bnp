#pragma once

#include <string>

namespace bnp {

struct CoreState {
	std::string file_path;
};

// Must be duplicated when creating a new game (without this component)
struct NewGameState {};

// Entities that must be serialized to save files
struct SaveState {};

}
