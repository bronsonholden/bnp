// Game manager is responsible for loading static data

#pragma once

#include <bnp/components/core_data.h>
#include <bnp/helpers/filesystem_helper.h>
#include <bnp/serializers/registry.hpp>
#include <bnp/game/components/matter.h>
#include <bnp/game/components/recipes.h>
#include <bnp/game/components/ships.h>
#include <bnp/game/components/universe.h>

#include <entt/entt.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>

namespace bnp {
namespace Game {
namespace Manager {

class GameManager {
public:
	GameManager() = default;

	void initialize(entt::registry& registry);

private:
	template <typename ...CoreDataSets>
	void load_core_data(entt::registry& registry, std::filesystem::path archive_path) {
		std::filesystem::path file_path = data_dir() / archive_path;
		std::ifstream is(file_path, std::ios::binary);
		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };
		(for_each_core_data_set<CoreDataSets>([&]<typename ...Components>() {
			bnp::deserialize<decltype(des), Components...>(des, registry);
		}), ...);
	}
};

}
}
}
