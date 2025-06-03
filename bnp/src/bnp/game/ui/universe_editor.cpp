#include <bnp/core/logger.hpp>
#include <bnp/components/state.h>
#include <bnp/helpers/filesystem_helper.h>
#include <bnp/game/ui/universe_editor.h>
#include <bnp/game/serializers/universe.hpp>
#include <bnp/game/components/navigation.h>
#include <bnp/game/managers/navigation_manager.h>

#include <imgui.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>

#include <filesystem>
#include <fstream>
#include <set>

namespace bnp {
namespace Game {
namespace UI {

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
		auto& celestial = view.get<Game::Component::Celestial>(entity);

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

		ser.object(celestial);
		ser.adapter().flush();
	}
}

void deserialize_systems(entt::registry& registry) {

}

void UniverseEditor::render(entt::registry& registry) {
	ImGui::Begin("Universe Editor");

	auto view = registry.view<Game::Component::CelestialMap>();

	if (view.front() != entt::null) {
		Game::Component::Celestial::ID celestial_id = view.get<Game::Component::CelestialMap>(view.front()).celestial_id;

		std::set<Game::Component::Celestial::ID> ids;
		auto celestials = registry.view<Game::Component::Celestial>();
		entt::entity celestial_entity = entt::null;

		for (auto entity : celestials) {
			auto& celestial = celestials.get<Game::Component::Celestial>(entity);

			if (celestial.id == celestial_id) {
				celestial_entity = entity;
			}

			ids.insert(celestial.id);
		}

		if (celestial_entity != entt::null) {
			auto& celestial = registry.get<Game::Component::Celestial>(celestial_entity);
			Game::Component::Celestial::ID original_id = celestial.id;

			if (ImGui::InputInt("Celestial ID", &celestial.id)) {
				while (ids.find(celestial.id) != ids.end() && celestial.id != original_id) celestial.id += 1;
				Game::Manager::NavigationManager().hide_celestial_map(registry);
				Game::Manager::NavigationManager().show_celestial_map(registry, celestial.id);
			}
		}
	}


	if (ImGui::Button("Save")) {
		Log::info("Saving core state");

		serialize_systems(registry);
		serialize_celestials(registry);
	}

	ImGui::End();
}

}
}
}
