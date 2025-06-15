#include <bnp/marshaling.hpp>
#include <bnp/game/ui/items_editor.h>
#include <bnp/game/components/inventory.h>
#include <bnp/game/components/matter.h>
#include <bnp/game/queries/inventory.h>
#include <bnp/game/serializers/inventory.hpp>

#include <imgui.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <filesystem>
#include <fstream>

namespace bnp {
namespace Game {
namespace UI {

void ItemsEditor::render(entt::registry& registry) {
	if (ImGui::Button("New item")) {
		// todo: get next ID from `ItemIndex`?
		Game::Component::Item::ID next_id = 1;
		auto items = registry.view<Game::Component::Item>();
		for (auto entity : items) {
			auto& item = items.get<Game::Component::Item>(entity);
			if (item.id >= next_id) next_id = item.id + 1;
		}

		registry.emplace<Game::Component::Item>(registry.create(), Game::Component::Item{
			.id = next_id,
			.name = "<New item>"
			});
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

	if (ImGui::Button("Save")) {
		save_to_file(registry);
	}

	ImGui::Separator();

	if (editing_item_entity != entt::null) {
		auto& item = registry.get<Game::Component::Item>(editing_item_entity);

		if (ImGui::Button("Back")) {
			editing_item_entity = entt::null;
		}
		else {
			bool is_chemical = registry.all_of<Game::Component::Chemical::ID>(editing_item_entity);
			auto chemical_id = is_chemical ? registry.get<Game::Component::Chemical::ID>(editing_item_entity) : 0;
			auto chemicals = registry.view<Game::Component::Chemical>();
			Game::Component::Chemical::ID selected_chemical_id = 0;
			entt::entity selected_chemical_entity = entt::null;

			for (auto entity : chemicals) {
				auto& chemical = chemicals.get<Game::Component::Chemical>(entity);

				if (chemical_id == chemical.id) {
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
			snprintf(label, 256, "##formula");
			if (ImGui::BeginCombo(label, preview.c_str())) {
				for (auto entity : chemicals) {
					auto& chemical = chemicals.get<Game::Component::Chemical>(entity);

					bool selected = chemical.id == selected_chemical_id;

					if (ImGui::Selectable(chemical.formula.c_str(), selected)) {
						registry.emplace_or_replace<Game::Component::Chemical::ID>(editing_item_entity, chemical.id);
					}

					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				if (ImGui::Selectable("<None>")) {
					registry.replace<Game::Component::Chemical::ID>(editing_item_entity, static_cast<Game::Component::Chemical::ID>(0));
				}

				ImGui::EndCombo();
			}
		}
	}
	else {
		std::vector<entt::entity> entities = std::move(Queries::get_item_entities_sorted_by_id(registry));

		if (entities.size() && ImGui::BeginTable("ItemTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
			ImGui::TableSetupColumn("ID");
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Actions");

			for (auto entity : entities) {
				auto& item = registry.get<Game::Component::Item>(entity);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				ImGui::Text("%d", item.id);
				ImGui::TableNextColumn();

				char name[256];
				snprintf(name, 256, "%s", item.name.c_str());
				char label[256];
				snprintf(label, 256, "##name:%d", item.id);
				if (ImGui::InputText(label, name, 256)) {
					item.name = name;
				}

				ImGui::TableNextColumn();

				{
					char label[256];
					snprintf(label, 256, "Edit##%d", item.id);
					if (ImGui::Button(label)) {
						editing_item_entity = entity;
					}
				}
			}

			ImGui::EndTable();
		}
	}
}

void ItemsEditor::save_to_file(entt::registry& registry) {
	save_component_set <
		// items
		SortedComponentSet <
		+[](entt::registry& registry, entt::entity a, entt::entity b) {
		auto& ca = registry.get<Component::Item>(a);
		auto& cb = registry.get<Component::Item>(b);
		return ca.id < cb.id;
		},
		Component::Item
		>
		> (registry, "items.bin");

}

}
}
}
