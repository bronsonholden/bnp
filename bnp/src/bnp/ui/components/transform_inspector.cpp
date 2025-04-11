#include <bnp/ui/components/transform_inspector.h>

namespace bnp {
	TransformInspector::TransformInspector(Node& _node) : node(_node) {

	}

	TransformInspector::~TransformInspector() {

	}

	void TransformInspector::render() {
		ImGui::Text("Transform");

		Transform& transform = node.get_component<Transform>();

		bool changed = false;
		float pos[3] = { transform.position.x, transform.position.y, transform.position.z };

		if (ImGui::Button("+X")) { pos[0] += 1.0f; changed = true; }
		ImGui::SameLine();
		if (ImGui::Button("+Y")) { pos[1] += 1.0f; changed = true; }
		ImGui::SameLine();
		if (ImGui::Button("+Z")) { pos[2] += 1.0f; changed = true; }

		if (ImGui::InputFloat3("Position", pos, "%.3f", ImGuiInputTextFlags_CharsScientific)) {
			changed = true;
		}

		if (ImGui::Button("-X")) { pos[0] -= 1.0f; changed = true; }
		ImGui::SameLine();
		if (ImGui::Button("-Y")) { pos[1] -= 1.0f; changed = true; }
		ImGui::SameLine();
		if (ImGui::Button("-Z")) { pos[2] -= 1.0f; changed = true; }

		// --- Scale (Input fields) ---
		float scale[3] = { transform.scale.x, transform.scale.y, transform.scale.z };
		if (ImGui::InputFloat3("Scale", scale, "%.3f")) {
			changed = true;
		}

		if (changed) {
			node.get_registry().patch<Transform>(node.get_entity_id(), [&pos, scale](Transform& transform) {
				transform.position = { pos[0], pos[1], pos[2] };
				transform.scale = { scale[0], scale[1], scale[2] };
				});
		}
	}
}
