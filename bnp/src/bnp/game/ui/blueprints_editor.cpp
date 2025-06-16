#include <bnp/core/logger.hpp>
#include <bnp/marshaling.hpp>
#include <bnp/game/components/ships.h>
#include <bnp/game/ui/blueprints_editor.h>
#include <bnp/game/serializers/ships.hpp>
#include <bnp/game/queries/recipes.h>
#include <bnp/game/queries/util.hpp>

#include <imgui.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/vector.h>
#include <bitsery/adapter/stream.h>
#include <fstream>

namespace bnp {
namespace Game {
namespace UI {

void BlueprintsEditor::initialize(entt::registry& registry) {
}

void BlueprintsEditor::render(entt::registry& registry) {
	if (ImGui::Button("Save")) {
		save_to_file(registry);
	}

	ImGui::BeginTabBar("Blueprint Types");

	if (ImGui::BeginTabItem("Ships")) {
		render_ship_blueprints_section(registry);
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Engines")) {
		render_engine_blueprints_section(registry);
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Fluid storage")) {
		render_fluid_storage_blueprints_section(registry);
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void BlueprintsEditor::render_ship_blueprints_section(entt::registry& registry) {
	if (edit_ship_blueprint_entity != entt::null) {
		render_edit_ship_blueprint_section(registry);
		return;
	}

	if (ImGui::Button("New ship blueprint")) {
		auto ship_blueprints = registry.view<Component::ShipBlueprint>();
		Component::ShipBlueprint::ID next_id = Queries::get_next_id<Component::ShipBlueprint>(registry);

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

void BlueprintsEditor::render_engine_blueprints_section(entt::registry& registry) {
	if (edit_engine_blueprint_entity != entt::null) {
		render_edit_engine_blueprint_section(registry);
		return;
	}

	if (ImGui::Button("New engine blueprint")) {
		Component::EngineBlueprint::ID next_id = Queries::get_next_id<Component::EngineBlueprint>(registry);

		registry.emplace<Component::EngineBlueprint>(registry.create(), Component::EngineBlueprint{
			.id = next_id,
			.name = "<New engine>",
			.mass = 1000,
			.effiency_factor = 1.0
			});
	}

	auto blueprints = registry.view<Component::EngineBlueprint>();

	if (ImGui::BeginTable("EngineBlueprints", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Actions");

		ImGui::TableNextColumn();
		ImGui::Text("ID");
		ImGui::TableNextColumn();
		ImGui::Text("Name");
		ImGui::TableNextColumn();
		ImGui::Text("Actions");

		for (auto entity : blueprints) {
			auto& blueprint = blueprints.get<Component::EngineBlueprint>(entity);

			ImGui::TableNextColumn();
			ImGui::Text("%d", blueprint.id);

			ImGui::TableNextColumn();
			ImGui::Text("%s", blueprint.name.c_str());

			ImGui::TableNextColumn();
			{
				char label[256];
				snprintf(label, 256, "Edit##%d", blueprint.id);
				if (ImGui::Button(label)) {
					edit_engine_blueprint_entity = entity;
				}
			}
		}

		ImGui::EndTable();
	}
}

void BlueprintsEditor::render_fluid_storage_blueprints_section(entt::registry& registry) {
	if (edit_fluid_storage_blueprint_entity != entt::null) {
		render_edit_fluid_storage_blueprint_section(registry);
		return;
	}

	if (ImGui::Button("New fluid storage blueprint")) {
		Component::FluidStorageBlueprint::ID next_id = Queries::get_next_id<Component::FluidStorageBlueprint>(registry);

		registry.emplace<Component::FluidStorageBlueprint>(registry.create(), Component::FluidStorageBlueprint{
			.id = next_id,
			.name = "<New fluid storage>",
			.mass = 1000
			});
	}

	auto blueprints = registry.view<Component::FluidStorageBlueprint>();

	if (ImGui::BeginTable("FluidStorageBlueprints", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("MaxVolume");
		ImGui::TableSetupColumn("MaxPressure");
		ImGui::TableSetupColumn("Actions");

		ImGui::TableNextColumn();
		ImGui::Text("ID");
		ImGui::TableNextColumn();
		ImGui::Text("Name");
		ImGui::TableNextColumn();
		ImGui::Text("Max volume (m^3)");
		ImGui::TableNextColumn();
		ImGui::Text("Max pressure (MPa)");
		ImGui::TableNextColumn();
		ImGui::Text("Actions");

		for (auto entity : blueprints) {
			auto& blueprint = blueprints.get<Component::FluidStorageBlueprint>(entity);

			ImGui::TableNextColumn();
			ImGui::Text("%d", blueprint.id);

			ImGui::TableNextColumn();
			ImGui::Text("%s", blueprint.name.c_str());

			ImGui::TableNextColumn();
			ImGui::Text("%.3f", blueprint.max_volume);

			ImGui::TableNextColumn();
			ImGui::Text("%.3f", blueprint.max_pressure);

			ImGui::TableNextColumn();
			{
				char label[256];
				snprintf(label, 256, "Edit##%d", blueprint.id);
				if (ImGui::Button(label)) {
					edit_fluid_storage_blueprint_entity = entity;
				}
			}
		}

		ImGui::EndTable();
	}
}

void BlueprintsEditor::render_edit_ship_blueprint_section(entt::registry& registry) {
	auto& blueprint = registry.get<Component::ShipBlueprint>(edit_ship_blueprint_entity);

	if (ImGui::Button("Back")) {
		edit_ship_blueprint_entity = entt::null;
		return;
	}

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
		Component::ShipSegment::ID next_id = Queries::get_next_id<Component::ShipSegment>(registry);
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

		ImGui::Text("ID: %d", segment.id);

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

void BlueprintsEditor::render_edit_engine_blueprint_section(entt::registry& registry) {
	if (ImGui::Button("Back")) {
		edit_engine_blueprint_entity = entt::null;
		return;
	}

	auto& blueprint = registry.get<Component::EngineBlueprint>(edit_engine_blueprint_entity);

	{
		char name[256];
		snprintf(name, 256, "%s", blueprint.name.c_str());
		if (ImGui::InputText("Name", name, 256)) {
			blueprint.name = name;
		}
	}

	ImGui::InputDouble("Mass", &blueprint.mass);
	ImGui::InputDouble("Efficiency factor", &blueprint.effiency_factor);

	int remove_index = -1;

	if (ImGui::TreeNode("Propulsion Recipes")) {
		if (ImGui::Button("Add recipe")) {
			blueprint.propulsion_recipes.push_back(0);
		}

		for (int i = 0; i < blueprint.propulsion_recipes.size(); ++i) {
			auto& recipe_id = blueprint.propulsion_recipes.at(i);

			std::string name = "<Select chemical recipe>";
			entt::entity chemical_entity = Queries::get_chemical_recipe_by_id(registry, recipe_id);

			if (chemical_entity != entt::null) {
				name = registry.get<Component::ChemicalRecipe>(chemical_entity).name;
			}

			{
				char label[256];
				snprintf(label, 256, "##recipe:%d", i);
				if (ImGui::BeginCombo(label, name.c_str())) {
					auto recipes = registry.view<Component::ChemicalRecipe>();
					for (auto entity : recipes) {
						auto& recipe = recipes.get<Game::Component::ChemicalRecipe>(entity);

						bool selected = recipe.id == recipe_id;

						if (ImGui::Selectable(recipe.name.c_str(), selected)) {
							recipe_id = recipe.id;
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
			}

		}

		if (remove_index > -1) {
			blueprint.propulsion_recipes.erase(blueprint.propulsion_recipes.begin() + remove_index);
		}

		ImGui::TreePop();
	}
}


void BlueprintsEditor::render_edit_fluid_storage_blueprint_section(entt::registry& registry) {
	if (ImGui::Button("Back")) {
		edit_fluid_storage_blueprint_entity = entt::null;
		return;
	}

	auto& blueprint = registry.get<Component::FluidStorageBlueprint>(edit_fluid_storage_blueprint_entity);

	{
		char name[256];
		snprintf(name, 256, "%s", blueprint.name.c_str());
		if (ImGui::InputText("Name", name, 256)) {
			blueprint.name = name;
		}
	}

	ImGui::InputDouble("Mass", &blueprint.mass);
	ImGui::InputDouble("Max volume (m^3)", &blueprint.max_volume);
	ImGui::InputDouble("Max pressure (MPa)", &blueprint.max_pressure);
}

void BlueprintsEditor::save_to_file(entt::registry& registry) {
	Marshaling::save_component_set <
		// ship blueprints
		SortedComponentSet <
		+[](entt::registry& registry, entt::entity a, entt::entity b) {
		auto& ca = registry.get<Component::ShipBlueprint>(a);
		auto& cb = registry.get<Component::ShipBlueprint>(b);
		return ca.id < cb.id;
		},
		Component::ShipBlueprint
		> ,

		// ship segments
		SortedComponentSet <
		+[](entt::registry& registry, entt::entity a, entt::entity b) {
		auto& ca = registry.get<Component::ShipSegment>(a);
		auto& cb = registry.get<Component::ShipSegment>(b);

		return ca.id < cb.id;
		},
		Component::ShipSegment
		> ,

		// engine blueprints
		SortedComponentSet <
		+[](entt::registry& registry, entt::entity a, entt::entity b) {
		auto& ca = registry.get<Component::EngineBlueprint>(a);
		auto& cb = registry.get<Component::EngineBlueprint>(b);

		return ca.id < cb.id;
		},
		Component::EngineBlueprint
		> ,

		// fluid storage blueprints
		SortedComponentSet <
		+[](entt::registry& registry, entt::entity a, entt::entity b) {
		auto& ca = registry.get<Component::FluidStorageBlueprint>(a);
		auto& cb = registry.get<Component::FluidStorageBlueprint>(b);

		return ca.id < cb.id;
		},
		Component::FluidStorageBlueprint
		>
		> (registry, "blueprints.bin");
}

}
}
}
