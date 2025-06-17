// Generic component editor, useful for static data which is represented
// by a single component.

#pragma once

#include <bnp/core/logger.hpp>
#include <bnp/components/reflection.h>
#include <bnp/game/queries/util.hpp>

#include <entt/entt.hpp>
#include <imgui.h>
#include <string>

namespace bnp {
namespace Game {
namespace UI {

template <typename Component>
class ComponentEditor {
public:
	void render(entt::registry& registry) {
		if (editing != entt::null) {
			auto& component = registry.get<Component>(editing);
			auto fields = Component::reflect_edit_fields();

			if (ImGui::Button("Back")) {
				editing = entt::null;
			}
			else {
				std::apply([&](auto&&... field) {
					(render_field(component, field.first, field.second), ...);
					}, fields);
			}
		}
		else {
			if (ImGui::Button("New entity")) {
				typename Component::ID next_id = Queries::get_next_id<Component>(registry);
				registry.emplace<Component>(registry.create(), Component{
					.id = next_id
					});
			}

			auto fields = Component::reflect_table_header_fields();
			auto view = registry.view<Component>();

			// add 1 for actions column
			int count = std::tuple_size<decltype(fields)>::value + 1;

			if (view.size() && ImGui::BeginTable("ComponentTable", count, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
				// define columns
				std::apply([&](auto&&... field) {
					(setup_table_column(field.first), ...);
					}, fields);

				ImGui::TableSetupColumn("Actions");

				// header row
				std::apply([&](auto&&... field) {
					([&] {
						ImGui::TableNextColumn();
						ImGui::Text(field.first);
						}(), ...);
					}, fields);

				ImGui::TableNextColumn();
				ImGui::Text("Actions");

				// data
				for (auto entity : view) {
					auto& component = view.get<Component>(entity);

					std::apply([&](auto&&... field) {
						([&] {
							ImGui::TableNextColumn();
							render_table_cell(component, field.first, field.second);
							}(), ...);
						}, fields);

					// actions column
					ImGui::TableNextColumn();
					char label[256];
					snprintf(label, 256, "Edit##%d", static_cast<int>(entity));
					if (ImGui::Button(label)) {
						editing = entity;
					}
				}

				ImGui::EndTable();
			}
		}
	}

private:
	entt::entity editing = entt::null;

	template <typename FieldType>
	void render_field(Component& c, const char* name, FieldType Component::* field) {
		if constexpr (std::is_same_v<FieldType, int>) {
			ImGui::InputInt(name, &(c.*field));
		}
		else if constexpr (std::is_same_v<FieldType, double>) {
			ImGui::InputDouble(name, &(c.*field));
		}
		else if constexpr (std::is_same_v<FieldType, float>) {
			ImGui::InputFloat(name, &(c.*field));
		}
		else if constexpr (std::is_same_v<FieldType, std::string>) {
			char value[256];
			snprintf(value, 256, "%s", (c.*field).c_str());
			if (ImGui::InputText(name, value, 256)) {
				(c.*field) = value;
			}
		}
		else {
			ImGui::TextColored(ImVec4(1, 1, 0.2, 1), "Unable to auto-generate input for %s, unrecognized type", name);
		}
	}

	void setup_table_column(const char* label) {
		ImGui::TableSetupColumn(label);
	}

	void render_table_header(const char* label) {
		ImGui::NextColumn();
		ImGui::Text(label);
	}

	template <typename FieldType>
	void render_table_cell(Component& c, const char* label, FieldType Component::* field) {
		ImGui::NextColumn();

		if constexpr (std::is_same_v<FieldType, int>) {
			ImGui::Text("%d", (c.*field));
		}
		else if constexpr (std::is_same_v<FieldType, double>) {
			ImGui::Text("%.10f", (c.*field));
		}
		else if constexpr (std::is_same_v<FieldType, float>) {
			ImGui::Text("%.5f", (c.*field));
		}
		else if constexpr (Reflection::is_effectively_uint32<FieldType>) {
			ImGui::Text("%u", (c.*field));
		}
		else if constexpr (std::is_same_v<FieldType, std::string>) {
			ImGui::Text("%s", (c.*field).c_str());
		}
		else {
			ImGui::TextColored(ImVec4(1, 1, 0.2, 1), "!!");
		}
	}
};


}
}
}
