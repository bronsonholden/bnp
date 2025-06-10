#include <bnp/helpers/filesystem_helper.h>
#include <bnp/game/components/ships.h>
#include <bnp/game/ui/blueprints_editor.h>
#include <bnp/game/serializers/ships.hpp>

#include <imgui.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/vector.h>
#include <bitsery/adapter/stream.h>
#include <fstream>

namespace bnp {
namespace Game {
namespace UI {

void BlueprintsEditor::initialize(entt::registry& registry) {
	load_from_file(registry, data_dir() / "blueprints.bin");
}

void BlueprintsEditor::render(entt::registry& registry) {
	if (ImGui::Button("Save")) {
		save_to_file(registry, data_dir() / "blueprints.bin");
	}

	ImGui::BeginTabBar("Blueprint Types");

	if (ImGui::BeginTabItem("Ships")) {
		render_ship_blueprints_section(registry);
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void BlueprintsEditor::render_ship_blueprints_section(entt::registry& registry) {
	if (edit_ship_blueprint_entity != entt::null) {
		auto& blueprint = registry.get<Component::ShipBlueprint>(edit_ship_blueprint_entity);
		if (ImGui::Button("Back")) {
			edit_ship_blueprint_entity = entt::null;
		}
		else {
			{
				char name[256];
				snprintf(name, 256, "%s", blueprint.name.c_str());
				if (ImGui::InputText("Name", name, 256)) {
					blueprint.name = name;
				}
			}

			{
				char manufacturer[256];
				snprintf(manufacturer, 256, "%s", blueprint.manufacturer.c_str());
				if (ImGui::InputText("Manufacturer", manufacturer, 256)) {
					blueprint.manufacturer = manufacturer;
				}
			}

			ImGui::InputDouble("Mass", &blueprint.mass);

			ImGui::Spacing();

			if (ImGui::Button("New segment")) {
				Component::ShipSegment::ID next_id = 1;
				auto view = registry.view<Component::ShipSegment>();
				for (auto entity : view) {
					auto& segment = view.get<Component::ShipSegment>(entity);
					if (segment.id >= next_id) next_id = segment.id + 1;
				}
				registry.emplace<Component::ShipSegment>(registry.create(), Component::ShipSegment{
					.ship_blueprint_id = blueprint.id,
					.id = next_id,
					.name = "<New segment>"
					});
			}

			std::vector<entt::entity> segment_entities;
			auto segments = registry.view<Component::ShipSegment>();
			for (auto entity : segments) {
				if (segments.get<Component::ShipSegment>(entity).ship_blueprint_id == blueprint.id) {
					segment_entities.push_back(entity);
				}
			}

			for (auto entity : segment_entities) {
				auto& segment = registry.get<Component::ShipSegment>(entity);
				ImGui::Separator();
				ImGui::Spacing();

				{
					char name[256];
					char label[256];
					snprintf(name, 256, "%s", segment.name.c_str());
					snprintf(label, 256, "Name##%d", segment.id);
					if (ImGui::InputText(label, name, 256)) {
						segment.name = name;
					}
				}

				int engine_slots = segment.engine_slots;
				if (ImGui::InputInt("Engine slots", &engine_slots)) {
					if (engine_slots > -1) segment.engine_slots = static_cast<uint32_t>(engine_slots);
				}

				int reactor_slots = segment.reactor_slots;
				if (ImGui::InputInt("Reactor slots", &reactor_slots)) {
					if (reactor_slots > -1) segment.reactor_slots = static_cast<uint32_t>(reactor_slots);
				}

				int solid_storage_slots = segment.solid_storage_slots;
				if (ImGui::InputInt("Solid storage slots", &solid_storage_slots)) {
					if (solid_storage_slots > -1) segment.solid_storage_slots = static_cast<uint32_t>(solid_storage_slots);
				}

				int fluid_storage_slots = segment.fluid_storage_slots;
				if (ImGui::InputInt("Solid storage slots", &fluid_storage_slots)) {
					if (fluid_storage_slots > -1) segment.fluid_storage_slots = static_cast<uint32_t>(fluid_storage_slots);
				}
			}
		}
	}
	else {
		if (ImGui::Button("New ship blueprint")) {
			auto ship_blueprints = registry.view<Component::ShipBlueprint>();
			Component::ShipBlueprint::ID next_id = 1;
			for (auto entity : ship_blueprints) {
				auto& blueprint = ship_blueprints.get<Component::ShipBlueprint>(entity);
				if (blueprint.id >= next_id) next_id = blueprint.id + 1;
			}

			registry.emplace<Component::ShipBlueprint>(registry.create(), Component::ShipBlueprint{
				.id = next_id
				});
		}

		auto ship_blueprints = registry.view<Component::ShipBlueprint>();

		entt::entity delete_entity = entt::null;

		if (ship_blueprints.size() && ImGui::BeginTable("Ship blueprints", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
			ImGui::TableSetupColumn("ID");
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Manufacturer");
			ImGui::TableSetupColumn("Actions");

			ImGui::TableNextColumn();
			ImGui::Text("ID");
			ImGui::TableNextColumn();
			ImGui::Text("Name");
			ImGui::TableNextColumn();
			ImGui::Text("Manufacturer");
			ImGui::TableNextColumn();
			ImGui::Text("Actions");

			for (auto entity : ship_blueprints) {
				auto& blueprint = ship_blueprints.get<Component::ShipBlueprint>(entity);

				ImGui::TableNextColumn();
				ImGui::Text("%d", blueprint.id);

				ImGui::TableNextColumn();
				ImGui::Text("%s", blueprint.name.c_str());

				ImGui::TableNextColumn();
				ImGui::Text("%s", blueprint.manufacturer.c_str());

				ImGui::TableNextColumn();
				{
					char label[256];
					snprintf(label, 256, "Edit##%d", blueprint.id);
					if (ImGui::Button(label)) {
						edit_ship_blueprint_entity = entity;
					}
				}
				ImGui::SameLine();
				{
					char label[256];
					snprintf(label, 256, "Delete##%d", blueprint.id);
					if (ImGui::Button(label)) {
						delete_entity = entity;
					}
				}
			}

			ImGui::EndTable();
		}

		if (delete_entity != entt::null) {
			registry.destroy(delete_entity);
		}
	}

}

void BlueprintsEditor::save_to_file(entt::registry& registry, std::filesystem::path file_path) {
	std::ofstream os(file_path, std::ios::binary);
	bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };

	if (!os.is_open()) return;

	{
		auto ship_blueprints = registry.view<Component::ShipBlueprint>();
		uint64_t count = ship_blueprints.size();
		ser.value8b(count);
		for (auto entity : ship_blueprints) {
			Component::ShipBlueprint ship_blueprint = ship_blueprints.get<Component::ShipBlueprint>(entity);
			ship_blueprint.version = ship_blueprint.latest_version;
			ser.object(ship_blueprint);
		}
	}

	{
		auto ship_segment_blueprints = registry.view<Component::ShipSegment>();
		uint64_t count = ship_segment_blueprints.size();
		ser.value8b(count);
		for (auto entity : ship_segment_blueprints) {
			Component::ShipSegment ship_segment = ship_segment_blueprints.get<Component::ShipSegment>(entity);
			ship_segment.version = ship_segment.latest_version;
			ser.object(ship_segment);
		}
	}
}

void BlueprintsEditor::load_from_file(entt::registry& registry, std::filesystem::path file_path) {
	std::ifstream is(file_path, std::ios::binary);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	if (!is.is_open()) return;

	// ship blueprints
	{
		uint64_t count;
		des.value8b(count);
		for (uint64_t i = 0; i < count; ++i) {
			Component::ShipBlueprint blueprint;
			des.object(blueprint);
			registry.emplace<Component::ShipBlueprint>(registry.create(), blueprint);
		}
	}

	// ship segments
	{
		uint64_t count;
		des.value8b(count);
		for (uint64_t i = 0; i < count; ++i) {
			Component::ShipSegment segment;
			des.object(segment);
			registry.emplace<Component::ShipSegment>(registry.create(), segment);
		}
	}
}

}
}
}
