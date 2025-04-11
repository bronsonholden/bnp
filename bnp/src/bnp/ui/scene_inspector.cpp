#include <bnp/ui/scene_inspector.h>

namespace bnp {

	SceneInspector::SceneInspector(Scene& scene)
		: scene(scene) {}

	std::optional<entt::entity> SceneInspector::get_selected_entity() const {
		return selected_entity;
	}

	std::optional<entt::entity> SceneInspector::get_inspected_entity() const {
		return inspected_entity;
	}

	void SceneInspector::render() {
		ImGui::Begin("Scene Inspector");

		if (ImGui::BeginTable("NodeTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Entity ID");
			ImGui::TableHeadersRow();

			auto& nodes = scene.get_nodes();

			for (const auto& [entity, node] : nodes) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				float row_height = ImGui::GetFrameHeight();

				ImGui::Text("<Unnamed node>");
				ImGui::TableNextColumn();

				bool is_selected = (selected_entity && *selected_entity == entity);
				char label[32];
				snprintf(label, sizeof(label), "%u", static_cast<uint32_t>(entity));

				if (ImGui::Selectable(label, is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0, ImGui::GetFrameHeight()))) {
					selected_entity = entity;
				}

				ImGui::TableNextColumn();

				if (ImGui::Button("Inspect")) {
					inspected_entity = entity;
				}
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}

}
