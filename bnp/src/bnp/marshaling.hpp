#pragma once

#include <bnp/helpers/filesystem_helper.h>
#include <bnp/serializers/registry.hpp>

#include <entt/entt.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <fstream>
#include <filesystem>

namespace bnp {

template <typename... ComponentSets>
void save_component_set(entt::registry& registry, std::filesystem::path archive_path) {
	std::filesystem::path file_path = data_dir() / archive_path;
	std::ofstream os(file_path, std::ios::binary);

	if (!os.is_open()) {
		Log::error("Unable to open data set file: %s", archive_path.string().c_str());
		return;
	}

	bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };

	(..., (
		unpack_component_set<ComponentSets>::apply([&]<typename... Components>() {
		constexpr bnp::Sorter* sorter = get_sorter_for_set<ComponentSets>();
		bnp::serialize<decltype(ser), Components...>(ser, registry, 1, sorter);
	})
		));
}

template <typename ...ComponentSets>
void load_component_set(entt::registry& registry, std::filesystem::path archive_path) {
	std::filesystem::path file_path = data_dir() / archive_path;
	std::ifstream is(file_path, std::ios::binary);

	if (!is.is_open()) {
		Log::error("Unable to open data set file: %s", archive_path.string().c_str());
		return;
	}

	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	(for_each_component_set<ComponentSets>([&]<typename ...Components>() {
		bnp::deserialize<decltype(des), Components...>(des, registry);
	}), ...);
}

}
