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

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <fstream>

namespace bnp {
namespace Game {
namespace UI {

void UniverseEditor::render(entt::registry& registry) {
	ImGui::Begin("Universe Editor");

	if (!registry.view<Game::Component::Galaxy>().size()) {
		load_from_file(registry, data_dir() / "universe.bin");
		Game::Manager::NavigationManager().show_galaxy_map(registry);
	}
	else {
		if (ImGui::Button("Save")) {
			save_to_file(registry, data_dir() / "universe.bin");
		}
	}

	ImGui::Separator();

	{
		auto view = registry.view<Game::Component::CelestialMap>();
		if (view.size()) {
			auto& celestial = view.get<Game::Component::CelestialMap>(view.front());
			render_celestial_editor(registry, celestial.celestial_id);
		}
	}

	{
		auto view = registry.view<Game::Component::SystemMap>();
		if (view.size()) {
			auto& system = view.get<Game::Component::SystemMap>(view.front());
			render_system_editor(registry, system.system_id);
		}
	}

	ImGui::End();
}

void UniverseEditor::save_to_file(entt::registry& registry, std::filesystem::path file_path) {
	std::ofstream os(file_path, std::ios::binary);
	bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };

	// universe
	{
		auto universes = registry.view<Game::Component::Universe>();
		auto count = universes.size();

		if (count > 1) {
			Log::warning("Multiple Universe components found");
		}
		else if (!count) {
			Log::warning("No Universe component found");
			return;
		}

		Game::Component::Universe universe(registry.get<Game::Component::Universe>(universes.front()));
		ser.object(universe);
		Log::info("Serialized universe");
	}

	// galaxies
	{
		auto galaxies = registry.view<Game::Component::Galaxy>();
		uint64_t count = galaxies.size();
		ser.value8b(count);
		for (auto entity : galaxies) {
			auto& galaxy = galaxies.get<Game::Component::Galaxy>(entity);
			if (!registry.valid(entity)) {
				Log::warning("Skipping invalid entity %d", static_cast<int>(entity));
				continue;
			}
			galaxy.version = galaxy.latest_version;
			ser.object(galaxy);
			Log::info("Serialized galaxy");
		}
	}

	// systems
	{
		auto systems = registry.view<Game::Component::System>();
		uint64_t count = systems.size();
		ser.value8b(count);

		std::vector<Game::Component::System::ID> system_ids;
		std::unordered_map<Game::Component::System::ID, entt::entity> entities;
		for (auto entity : systems) {
			auto& system = systems.get<Game::Component::System>(entity);
			entities.emplace(system.id, entity);
			system_ids.push_back(system.id);
		}

		std::sort(system_ids.begin(), system_ids.end());

		for (auto id : system_ids) {
			auto entity = entities.at(id);
			if (!registry.valid(entity)) {
				Log::warning("Skipping invalid entity %d", static_cast<int>(entity));
				continue;
			}
			Game::Component::System system = registry.get<Game::Component::System>(entity);
			system.version = system.latest_version;
			ser.object(system);
			Log::info("Serialized system (id=%d,version=%d,name=%s)", system.id, system.version, system.name.c_str());
		}
	}

	// celestials
	{
		auto celestials = registry.view<Game::Component::Celestial, Planet2D>();
		uint64_t count = 0;
		for (auto _ : celestials) count++;
		ser.value8b(count);

		std::vector<Game::Component::Celestial::ID> celestial_ids;
		std::unordered_map<Game::Component::Celestial::ID, entt::entity> entities;
		for (auto entity : celestials) {
			auto& celestial = celestials.get<Game::Component::Celestial>(entity);
			if (!registry.valid(entity)) {
				Log::warning("Skipping invalid entity %d", static_cast<int>(entity));
				continue;
			}
			entities.emplace(celestial.id, entity);
			celestial_ids.push_back(celestial.id);
		}

		std::sort(celestial_ids.begin(), celestial_ids.end());

		for (auto id : celestial_ids) {
			auto entity = entities.at(id);
			Game::Component::Celestial celestial = registry.get<Game::Component::Celestial>(entity);
			Planet2D planet = registry.get<Planet2D>(entity);
			celestial.version = celestial.latest_version;
			ser.object(celestial);
			ser.object(planet);
			Log::info("Serialized celestial (id=%d,version=%d,name=%s)", celestial.id, celestial.version, celestial.name.c_str());
		}
	}

	ser.adapter().flush();
}

void UniverseEditor::load_from_file(entt::registry& registry, std::filesystem::path file_path) {
	std::ifstream is(file_path, std::ios::binary);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	// universe
	{
		Game::Component::Universe universe;
		des.object(universe);
		Log::info("Deserialized universe");
		registry.emplace<Game::Component::Universe>(registry.create(), universe);
	}

	// Galaxy
	{
		uint64_t count;
		des.value8b(count);
		for (uint64_t i = 0; i < count; ++i) {
			entt::entity entity = registry.create();
			Game::Component::Galaxy galaxy;
			des.object(galaxy);
			Log::info("Deserialized galaxy");
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
			Log::info("Deserialized system (id=%d,version=%d,name=%s)", system.id, system.version, system.name.c_str());
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
			celestial.version = celestial.latest_version;
			registry.emplace<Game::Component::Celestial>(entity, celestial);
			Planet2D planet;
			des.object(planet);
			Log::info("Deserialized celestial (id=%d,version=%d,name=%s)", celestial.id, celestial.version, celestial.name.c_str());
			registry.emplace<Planet2D>(entity, planet);
		}
	}
}

void UniverseEditor::render_system_editor(entt::registry& registry, Game::Component::System::ID system_id) {
	auto celestials = registry.view<Game::Component::Celestial>();

	Game::Component::Celestial::ID next_id = 1;
	for (auto entity : celestials) {
		auto& celestial = celestials.get<Game::Component::Celestial>(entity);
		if (celestial.id >= next_id) next_id = celestial.id + 1;
	}

	if (ImGui::Button("New celestial")) {
		entt::entity celestial_entity = registry.create();

		Game::Component::Celestial celestial{
			.id = next_id,
			.system_id = system_id,
			.name = "<Celestial>",
			.orbit_radius = 1.0,
			.initial_orbit_progression = 0.0,
			.orbit_duration = 24.0 * 3600.0 * 365.0,
			.initial_rotate_progression = 0.0,
			.rotate_duration = 24.0 * 3600.0
		};

		registry.emplace<Game::Component::Celestial>(celestial_entity, celestial);
		registry.emplace<Planet2D>(celestial_entity);
	}

	ImGui::Separator();

	bool changed = false;

	for (auto entity : celestials) {
		auto& celestial = celestials.get<Game::Component::Celestial>(entity);

		if (celestial.system_id != system_id) continue;

		if (ImGui::TreeNode(celestial.name.c_str())) {
			ImGui::Indent();
			changed = ImGui::InputDouble("Orbit Radius", &celestial.orbit_radius, 0.1) || changed;
			changed = ImGui::InputDouble("Initial Orbit Progression", &celestial.initial_orbit_progression, 0.1) || changed;
			changed = ImGui::InputDouble("Orbit Duration", &celestial.orbit_duration) || changed;
			ImGui::Text("Orbit duration (days): %2.3f", celestial.orbit_duration / (24.0 * 3600.0));
			changed = ImGui::InputDouble("Initial Rotate Progression", &celestial.initial_rotate_progression) || changed;
			changed = ImGui::InputDouble("Rotate Duration", &celestial.rotate_duration) || changed;
			ImGui::Text("Rotate duration (hours): %2.3f", celestial.rotate_duration / 3600.0);
			ImGui::Unindent();
			ImGui::TreePop();
		}
	}

	if (changed) {
		Game::Manager::NavigationManager().hide_system_map(registry);
		Game::Manager::NavigationManager().show_system_map(registry, system_id);
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

	ImGui::Spacing();

	ImGui::Text("Celestial");
	ImGui::Indent();
	ImGui::InputDouble("Orbit Radius", &celestial.orbit_radius);
	ImGui::InputDouble("Initial Orbit Progression", &celestial.initial_orbit_progression);
	ImGui::InputDouble("Orbit Duration", &celestial.orbit_duration);
	ImGui::Text("Orbit duration (days): %2.3f", celestial.orbit_duration / (24.0 * 3600.0));
	ImGui::InputDouble("Initial Rotate Progression", &celestial.initial_rotate_progression);
	ImGui::InputDouble("Rotate Duration", &celestial.rotate_duration);
	ImGui::Text("Rotate duration (hours): %2.3f", celestial.rotate_duration / 3600.0);
	ImGui::InputDouble("Mass", &celestial.mass);
	ImGui::InputDouble("Radius", &celestial.radius);
	ImGui::Text("g: %2.6f m/s^2", celestial.g());

	if (ImGui::TreeNode("Atmosphere")) {
		if (ImGui::Button("New chemical")) {
			celestial.atmosphere.push_back({ 0, 0 });
		}

		ImGui::Text("Atmosphere components: %d", celestial.atmosphere.size());
		ImGui::Text("Gravity (m/s^2): %.5f", celestial.g());

		double column_mass = 0;
		for (auto& entry : celestial.atmosphere) {
			column_mass += entry.first;
		}

		ImGui::Text("Atmospheric column mass (trillions kg): %.2f", column_mass / 1e12);

		double atmospheric_pressure_Pa = (column_mass * celestial.g()) / (4.0 * std::numbers::pi * celestial.radius * celestial.radius);
		double atmospheric_pressure_atm = atmospheric_pressure_Pa / 101325.0; // Pa to atm

		ImGui::Text("Surface atmosphere pressure (atm): %.5f", atmospheric_pressure_atm);

		if (celestial.atmosphere.size() && ImGui::BeginTable("AtmosphereTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
			ImGui::TableSetupColumn("Formula");
			ImGui::TableSetupColumn("Mass (trillions kg)");
			ImGui::TableSetupColumn("%");

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Formula");
			ImGui::TableNextColumn();
			ImGui::Text("Mass (trillions kg)");
			ImGui::TableNextColumn();
			ImGui::Text("Percentage");

			int remove_index = -1;

			for (int i = 0; i < celestial.atmosphere.size(); ++i) {
				auto& entry = celestial.atmosphere.at(i);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				{
					auto chemicals = registry.view<Game::Component::Chemical>();
					Game::Component::Chemical::ID selected_chemical_id = 0;
					entt::entity selected_chemical_entity = entt::null;

					for (auto entity : chemicals) {
						auto& chemical = chemicals.get<Game::Component::Chemical>(entity);

						if (entry.second == chemical.id) {
							selected_chemical_entity = entity;
							break;
						}
					}

					std::string preview = "Select chemical";

					if (selected_chemical_entity != entt::null) {
						auto& chemical = registry.get<Game::Component::Chemical>(selected_chemical_entity);
						preview = chemical.formula;
						selected_chemical_id = chemical.id;
					}

					char label[256];
					snprintf(label, 256, "##formula:%d", i);
					if (ImGui::BeginCombo(label, preview.c_str())) {
						for (auto entity : chemicals) {
							auto& chemical = chemicals.get<Game::Component::Chemical>(entity);

							bool selected = chemical.id == selected_chemical_id;

							if (ImGui::Selectable(chemical.formula.c_str(), selected)) {
								entry.second = chemical.id;
							}

							if (selected) {
								ImGui::SetItemDefaultFocus();
							}
						}

						if (ImGui::Selectable("<Remove>")) {
							remove_index = i;
						}

						ImGui::EndCombo();
					}

					ImGui::TableNextColumn();
					{
						char label[256];
						snprintf(label, 256, "##mass:%d", i);
						double value = entry.first / 1e12;
						if (ImGui::InputDouble(label, &value)) {
							entry.first = value * 1e12;
						}
					}

					ImGui::TableNextColumn();
					if (selected_chemical_entity != entt::null) {
						auto& chemical = registry.get<Game::Component::Chemical>(selected_chemical_entity);

						if (column_mass > 0.0) {
							ImGui::Text("%.7f%%", (entry.first / column_mass) * 100.0);
						}
						else {
							ImGui::Text("-");
						}
					}
				}

			}

			ImGui::EndTable();

			if (remove_index > -1) {
				celestial.atmosphere.erase(celestial.atmosphere.begin() + remove_index);
			}
		}

		ImGui::TreePop();
	}

	ImGui::Unindent();

	ImGui::Spacing();

	ImGui::Text("Planet2D");
	ImGui::Indent();
	ImGui::InputFloat("Noise radius", &planet.noise_radius);
	ImGui::InputFloat("Noise seed", &planet.noise_seed);
	ImGui::InputFloat3("Axis", &planet.axis.x);
	ImGui::InputFloat3("Sun direction", &planet.sun_direction.x);
	ImGui::InputFloat("Water depth", &planet.water_depth);
	ImGui::InputFloat("Coast depth", &planet.coast_depth);
	ImGui::InputFloat("Mainland depth", &planet.mainland_depth);

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
