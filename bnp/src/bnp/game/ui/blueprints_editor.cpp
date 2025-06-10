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
		}
	}
	else {
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
					snprintf(label, 256, "Delete##%d", blueprint.id);
					if (ImGui::Button(label)) {
						delete_entity = entity;
					}
				}
				{
					char label[256];
					snprintf(label, 256, "Edit##%d", blueprint.id);
					if (ImGui::Button(label)) {
						edit_ship_blueprint_entity = entity;
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
			auto& ship_blueprint = ship_blueprints.get<Component::ShipBlueprint>(entity);
			ser.object(ship_blueprint);
		}
	}
}

void BlueprintsEditor::load_from_file(entt::registry& registry, std::filesystem::path file_path) {
	std::ifstream is(file_path, std::ios::binary);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	if (!is.is_open()) return;

	{
		uint64_t count;
		des.value8b(count);
		for (uint64_t i = 0; i < count; ++i) {
			Component::ShipBlueprint blueprint;
			des.object(blueprint);
			registry.emplace<Component::ShipBlueprint>(registry.create(), blueprint);
		}
	}
}

}
}
}
