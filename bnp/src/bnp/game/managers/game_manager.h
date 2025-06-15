// Game manager is responsible for loading static data

#pragma once

#include <bnp/components/core_data.h>
#include <bnp/helpers/filesystem_helper.h>
#include <bnp/serializers/registry.hpp>

#include <entt/entt.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <filesystem>
#include <fstream>

namespace bnp {
namespace Game {
namespace Manager {

// todo: this can probably be moved out of game-specific code to engine code
class GameManager {
public:
	GameManager() = default;

	void initialize(entt::registry& registry);

private:
	template <typename ...CoreDataSets>
	void load_core_data(entt::registry& registry, std::filesystem::path archive_path) {
		std::filesystem::path file_path = data_dir() / archive_path;
		std::ifstream is(file_path, std::ios::binary);
		if (!is.is_open()) {
			Log::error("Unable to find data set file: %s", archive_path.string().c_str());
			return;
		}
		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };
		(for_each_core_data_set<CoreDataSets>([&]<typename ...Components>() {
			bnp::deserialize<decltype(des), Components...>(des, registry);
		}), ...);
	}
};

}
}
}
