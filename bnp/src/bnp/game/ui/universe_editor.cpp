#include <bnp/core/logger.hpp>
#include <bnp/components/state.h>
#include <bnp/components/world.h>
#include <bnp/helpers/filesystem_helper.h>
#include <bnp/serializers/world.hpp>
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
	auto view = registry.view<Game::Component::Celestial, Planet2D, CoreState>();

	for (auto entity : view) {
		auto& state = view.get<CoreState>(entity);
		auto& celestial = view.get<Game::Component::Celestial>(entity);
		auto& planet = view.get<Planet2D>(entity);

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
		ser.object(planet);
		ser.adapter().flush();
	}
}

void deserialize_systems(entt::registry& registry) {

}

void UniverseEditor::render(entt::registry& registry) {
	ImGui::Begin("Universe Editor");

	if (!registry.view<Game::Component::Galaxy>().size()) {
		if (ImGui::Button("Load")) {
			load_from_file(registry, data_dir() / "universe.bin");
		}
	}
	else {
		if (ImGui::Button("Save")) {
			save_to_file(registry, data_dir() / "universe.bin");
		}
	}

	ImGui::Separator();

	auto view = registry.view<Game::Component::CelestialMap>();
	if (view.size()) {
		auto& celestial = view.get<Game::Component::CelestialMap>(view.front());
		render_celestial_editor(registry, celestial.celestial_id);
	}

	ImGui::End();
}

void UniverseEditor::save_to_file(entt::registry& registry, std::filesystem::path file_path) {
	std::ofstream os(file_path);
	bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };

	// galaxies
	{
		auto galaxies = registry.view<Game::Component::Galaxy>();
		uint64_t count = galaxies.size();
		ser.value8b(galaxies.size());
		for (auto entity : galaxies) {
			auto& galaxy = galaxies.get<Game::Component::Galaxy>(entity);
			ser.object(galaxy);
		}
	}

	// systems
	{
		auto systems = registry.view<Game::Component::System>();
		uint64_t count = systems.size();
		ser.value8b(systems.size());
		for (auto entity : systems) {
			auto& system = systems.get<Game::Component::System>(entity);
			ser.object(system);
		}
	}

	// celestials
	{
		auto celestials = registry.view<Game::Component::Celestial, Planet2D>();
		uint64_t count = 0;
		for (auto _ : celestials) count++;
		ser.value8b(count);
		for (auto entity : celestials) {
			auto& celestial = celestials.get<Game::Component::Celestial>(entity);
			auto& planet = celestials.get<Planet2D>(entity);
			ser.object(celestial);
			ser.object(planet);
		}
	}
}

void UniverseEditor::load_from_file(entt::registry& registry, std::filesystem::path file_path) {
	std::ifstream is(file_path);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	// Galaxy
	{
		uint64_t count;
		des.value8b(count);
		for (uint64_t i = 0; i < count; ++i) {
			entt::entity entity = registry.create();
			Game::Component::Galaxy galaxy;
			des.object(galaxy);
			registry.emplace<Game::Component::Galaxy>(entity, galaxy);
		}
	}

	// systems
	{
		uint64_t count;
		des.value8b(count);
		for (uint64_t i = 0; i < count; ++i) {
			entt::entity entity = registry.create();
			Game::Component::System system;
			des.object(system);
			registry.emplace<Game::Component::System>(entity, system);
		}
	}

	// celestials
	{
		uint64_t count;
		des.value8b(count);
		for (uint64_t i = 0; i < count; ++i) {
			entt::entity entity = registry.create();
			Game::Component::Celestial celestial;
			des.object(celestial);
			registry.emplace<Game::Component::Celestial>(entity, celestial);
			Planet2D planet;
			des.object(planet);
			registry.emplace<Planet2D>(entity, planet);
		}
	}
}

void UniverseEditor::render_celestial_editor(entt::registry& registry, Game::Component::Celestial::ID celestial_id) {
	auto celestials = registry.view<Game::Component::Celestial>();
	entt::entity celestial_entity = entt::null;

	for (auto entity : celestials) {
		auto& celestial = celestials.get<Game::Component::Celestial>(entity);

		if (celestial.id == celestial_id) {
			celestial_entity = entity;
		}
	}

	if (celestial_entity == entt::null) return;

	auto& celestial = registry.get<Game::Component::Celestial>(celestial_entity);

	if (!registry.all_of<Planet2D>(celestial_entity)) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "No Planet2D component found");
		return;
	}

	auto& planet = registry.get<Planet2D>(celestial_entity);

	ImGui::Text("Celestial ID: %d", celestial.id);

	char name[256];
	snprintf(name, 256, "%s", celestial.name.c_str());
	if (ImGui::InputText("Name", name, 256, 0)) {
		celestial.name = name;
	}

	ImGui::Text("Celestial");
	ImGui::Indent();
	ImGui::InputDouble("Orbit Radius", &celestial.orbit_radius);
	ImGui::InputDouble("Initial Orbit Progression", &celestial.initial_orbit_progression);
	ImGui::Text("Orbit progression: %3.10f", celestial.orbit_progression);
	ImGui::InputDouble("Orbit Duration", &celestial.orbit_duration);
	ImGui::InputDouble("Initial Rotate Progression", &celestial.initial_rotate_progression);
	ImGui::Text("Rotate progression: %3.10f", celestial.rotate_progression);
	ImGui::InputDouble("Rotate Duration", &celestial.rotate_duration);
	ImGui::InputFloat("Noise radius", &planet.noise_radius);
	ImGui::InputFloat("Noise seed", &planet.noise_seed);
	ImGui::InputFloat3("Axis", &planet.axis.x);
	ImGui::InputFloat3("Sun direction", &planet.sun_direction.x);
	ImGui::InputFloat("Water depth", &planet.water_depth);
	ImGui::InputFloat("Coast depth", &planet.coast_depth);
	ImGui::InputFloat("Mainland depth", &planet.mainland_depth);
	ImGui::Unindent();

	ImGui::Text("Planet2D");
	ImGui::Indent();
	if (ImGui::TreeNode("Water color")) {
		ImGui::ColorPicker3("Water color", &planet.water_color.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Coast color")) {
		ImGui::ColorPicker3("Coast color", &planet.coast_color.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Mainland color")) {
		ImGui::ColorPicker3("Mainland color", &planet.mainland_color.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Mountain color")) {
		ImGui::ColorPicker3("Mountain color", &planet.mountain_color.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Cloud color")) {
		ImGui::ColorPicker3("Cloud color", &planet.cloud_color.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Ice cap color")) {
		ImGui::ColorPicker3("Ice cap color", &planet.ice_cap_color.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Crater color")) {
		ImGui::ColorPicker3("Crater color", &planet.crater_color.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Crater rim color")) {
		ImGui::ColorPicker3("Crater rim color", &planet.crater_rim_color.x);
		ImGui::TreePop();
	}

	ImGui::InputFloat("Cloud depth", &planet.cloud_depth);
	ImGui::InputFloat("Cloud radius", &planet.cloud_radius);
	ImGui::InputFloat("Cloud radius equator exp", &planet.cloud_radius_equator_exp);
	ImGui::InputFloat("Cloud banding eqautor exp", &planet.cloud_banding_equator_exp);

	ImGui::InputFloat("Crater step", &planet.crater_step);
	ImGui::InputInt("Num craters", &planet.num_craters);

	ImGui::InputFloat("Ice cap dot min", &planet.ice_cap_min);
	ImGui::InputFloat("Ice cap dot max", &planet.ice_cap_max);

	ImGui::Unindent();

}

}
}
}
