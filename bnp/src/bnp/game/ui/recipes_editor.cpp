#include <bnp/core/logger.hpp>
#include <bnp/marshaling.hpp>
#include <bnp/game/ui/recipes_editor.h>
#include <bnp/game/components/recipes.h>
#include <bnp/game/serializers/recipes.hpp>
#include <bnp/game/queries/chemicals.h>
#include <bnp/game/queries/util.hpp>

#include <imgui.h>
#include <bitsery/bitsery.h>
#include <bitsery/traits/vector.h>
#include <bitsery/adapter/stream.h>

namespace bnp {
namespace Game {
namespace UI {

void RecipesEditor::initialize(entt::registry& registry) {
}

void RecipesEditor::render(entt::registry& registry) {
	if (ImGui::Button("Save")) {
		save_to_file(registry);
	}

	ImGui::BeginTabBar("Recipe Types");

	if (ImGui::BeginTabItem("Chemical")) {
		render_chemical_recipes_section(registry);
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void RecipesEditor::render_chemical_recipes_section(entt::registry& registry) {
	if (edit_chemical_recipe_entity != entt::null) {
		render_edit_chemical_recipe_section(registry);
		return;
	}

	if (ImGui::Button("New chemical recipe")) {
		Component::ChemicalRecipe::ID next_id = Queries::get_next_id<Component::ChemicalRecipe>(registry);
		registry.emplace<Component::ChemicalRecipe>(registry.create(), Component::ChemicalRecipe{
			.id = next_id,
			.name = "<Chemical recipe>",
			.energy = 0
			});
	}

	auto recipes = registry.view<Component::ChemicalRecipe>();

	if (recipes.size() && ImGui::BeginTable("Chemical Recipes", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Actions");

		ImGui::TableNextColumn();
		ImGui::Text("ID");
		ImGui::TableNextColumn();
		ImGui::Text("Name");
		ImGui::TableNextColumn();
		ImGui::Text("Actions");

		for (auto entity : recipes) {
			auto& recipe = recipes.get<Component::ChemicalRecipe>(entity);

			ImGui::TableNextColumn();
			ImGui::Text("%d", recipe.id);

			ImGui::TableNextColumn();
			ImGui::Text("%s", recipe.name.c_str());

			ImGui::TableNextColumn();
			{
				char label[256];
				snprintf(label, 256, "Edit##%d", recipe.id);
				if (ImGui::Button(label)) {
					edit_chemical_recipe_entity = entity;
				}
			}
		}

		ImGui::EndTable();
	}
}

void RecipesEditor::render_edit_chemical_recipe_section(entt::registry& registry) {
	if (ImGui::Button("Back")) {
		edit_chemical_recipe_entity = entt::null;
		return;
	}

	auto& recipe = registry.get<Component::ChemicalRecipe>(edit_chemical_recipe_entity);

	ImGui::Text("ID: %d", recipe.id);

	{
		char name[256];
		snprintf(name, 256, "%s", recipe.name.c_str());
		if (ImGui::InputText("Name", name, 256)) {
			recipe.name = name;
		}
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Inputs")) {
		if (ImGui::Button("New input")) {
			recipe.inputs.push_back({ 0, 0 });
		}

		int remove_index = -1;

		for (int i = 0; i < recipe.inputs.size(); ++i) {
			auto& [ratio, chemical_id] = recipe.inputs.at(i);

			std::string name = "<Select chemical>";
			entt::entity chemical_entity = Queries::get_chemical_by_id(registry, chemical_id);

			if (chemical_entity != entt::null) {
				name = registry.get<Component::Chemical>(chemical_entity).name;
			}

			{
				char label[256];
				snprintf(label, 256, "##formula:%d", i);
				if (ImGui::BeginCombo(label, name.c_str())) {
					auto chemicals = registry.view<Component::Chemical>();
					for (auto entity : chemicals) {
						auto& chemical = chemicals.get<Game::Component::Chemical>(entity);

						bool selected = chemical.id == chemical_id;

						if (ImGui::Selectable(chemical.formula.c_str(), selected)) {
							chemical_id = chemical.id;
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

			ImGui::SameLine();

			{
				char label[256];
				snprintf(label, 256, "Ratio##%d", i);
				ImGui::InputDouble(label, &ratio);
			}
		}

		if (remove_index > -1) {
			recipe.inputs.erase(recipe.inputs.begin() + remove_index);
		}

		double ratio_total = 0;
		for (auto& [ratio, _] : recipe.inputs) ratio_total += ratio;

		if (std::abs(ratio_total - 1.0) > 0.00001) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "WARNING: Recipe input ratios should be ~= 1.0, currently: %0.5f", ratio_total);
		}

		ImGui::TreePop();
	}

	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Outputs")) {
		if (ImGui::Button("New output")) {
			recipe.outputs.push_back({ 0, 0 });
		}

		int remove_index = -1;

		for (int i = 0; i < recipe.outputs.size(); ++i) {
			auto& [ratio, chemical_id] = recipe.outputs.at(i);

			std::string name = "<Select chemical>";
			entt::entity chemical_entity = Queries::get_chemical_by_id(registry, chemical_id);

			if (chemical_entity != entt::null) {
				name = registry.get<Component::Chemical>(chemical_entity).name;
			}

			{
				char label[256];
				snprintf(label, 256, "##formula:%d", i);
				if (ImGui::BeginCombo(label, name.c_str())) {
					auto chemicals = registry.view<Component::Chemical>();
					for (auto entity : chemicals) {
						auto& chemical = chemicals.get<Game::Component::Chemical>(entity);

						bool selected = chemical.id == chemical_id;

						if (ImGui::Selectable(chemical.formula.c_str(), selected)) {
							chemical_id = chemical.id;
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

			ImGui::SameLine();

			{
				char label[256];
				snprintf(label, 256, "Ratio##%d", i);
				ImGui::InputDouble(label, &ratio);
			}
		}

		if (remove_index > -1) {
			recipe.outputs.erase(recipe.outputs.begin() + remove_index);
		}

		double ratio_total = 0;
		for (auto& [ratio, _] : recipe.outputs) ratio_total += ratio;

		if (std::abs(ratio_total - 1.0) > 0.00001) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "WARNING: Recipe output ratios should be ~= 1.0, currently: %0.5f", ratio_total);
		}

		ImGui::TreePop();
	}

	ImGui::InputDouble("Energy output (MJ/kg)", &recipe.energy);
	ImGui::InputDouble("Efficiency", &recipe.efficiency);
	ImGui::InputDouble("Protons emitted (per kg)", &recipe.protons_emitted);
	ImGui::InputDouble("Neutrons emitted (per kg)", &recipe.neutrons_emitted);

}

void RecipesEditor::save_to_file(entt::registry& registry) {
	save_component_set <
		// chemicals
		SortedComponentSet <
		+[](entt::registry& registry, entt::entity a, entt::entity b) {
		auto& ca = registry.get<Component::ChemicalRecipe>(a);
		auto& cb = registry.get<Component::ChemicalRecipe>(b);
		return ca.id < cb.id;
		},
		Component::ChemicalRecipe
		>
		> (registry, "chemicals.bin");
}

}
}
}
