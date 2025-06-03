#include <bnp/core/logger.hpp>
#include <bnp/components/state.h>
#include <bnp/helpers/filesystem_helper.h>
#include <bnp/game/ui/core_state_inspector.h>
#include <bnp/game/serializers/universe.hpp>

#include <imgui.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>

#include <filesystem>
#include <fstream>

namespace bnp {

void serialize_systems(entt::registry& registry) {
	auto view = registry.view<Game::Component::System, CoreState>();

	for (auto entity : view) {
		auto& state = view.get<CoreState>(entity);
		auto& system = view.get<Game::Component::System>(entity);

		std::filesystem::path file_path = data_dir() / state.file_path;
		std::filesystem::path dir_path = file_path.parent_path();

		if (!std::filesystem::exists(dir_path)) {
			std::filesystem::create_directories(dir_path);
		}

		std::ofstream os(file_path, std::ios::binary);

		if (!os.is_open()) {
			Log::error_wide(L"Unable to open stream: %s", file_path.c_str());
			continue;
		}

		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };

		ser.object(system);
		ser.adapter().flush();
	}
}

void serialize_celestials(entt::registry& registry) {
	auto view = registry.view<Game::Component::Celestial, CoreState>();

	for (auto entity : view) {
		auto& state = view.get<CoreState>(entity);
		auto& system = view.get<Game::Component::Celestial>(entity);

		std::filesystem::path file_path = data_dir() / state.file_path;
		std::filesystem::path dir_path = file_path.parent_path();

		if (!std::filesystem::exists(dir_path)) {
			std::filesystem::create_directories(dir_path);
		}

		std::ofstream os(file_path, std::ios::binary);

		if (!os.is_open()) {
			Log::error_wide(L"Unable to open stream: %s", file_path.c_str());
			continue;
		}

		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };

		ser.object(system);
		ser.adapter().flush();
	}
}

void deserialize_systems(entt::registry& registry) {

}

void CoreStateInspector::render(entt::registry& registry) {
	ImGui::Begin("Core State");

	if (ImGui::Button("Save")) {
		Log::info("Saving core state");

		serialize_systems(registry);
		serialize_celestials(registry);
	}

	ImGui::End();
}
}
