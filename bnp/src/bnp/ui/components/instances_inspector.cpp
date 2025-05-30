#include <bnp/ui/components/instances_inspector.h>

namespace bnp {

InstancesInspector::InstancesInspector(Node& _node)
	: node(_node)
{
}

void InstancesInspector::render() {
	static int selected_index = 0;

	ImGui::Begin("Instances Inspector");

	Instances& instances = node.get_component<Instances>();

	const int count = static_cast<int>(instances.transforms.size());
	ImGui::Text("Total Instances: %d", count);

	if (count == 0) {
		ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "No instances available.");
		ImGui::End();
		return;
	}

	// Input for selecting instance index
	ImGui::InputInt("Selected Instance", &selected_index);
	if (selected_index < 0) selected_index = 0;
	if (selected_index >= count) selected_index = count - 1;

	ImGui::Separator();

	// Show transform inspector for selected instance
	ImGui::Text("Editing Instance #%d", selected_index + 1);

	Transform transform = instances.transforms[selected_index];

	bool changed = render_instance_transform(transform);

	size_t update_at_index = selected_index;

	if (changed) {
		node.get_registry().patch<Instances>(node.get_entity_id(), [&transform, update_at_index](Instances& instances) {
			Transform& t = instances.transforms.at(update_at_index);
			t.position.x = transform.position.x;
			t.position.y = transform.position.y;
			t.position.z = transform.position.z;
			t.rotation.x = transform.rotation.x;
			t.rotation.y = transform.rotation.y;
			t.rotation.z = transform.rotation.z;
			t.rotation.w = transform.rotation.w;
			t.scale.x = transform.scale.x;
			t.scale.y = transform.scale.y;
			t.scale.z = transform.scale.z;
			t.dirty = true;
			instances.dirty = true;
			instances.update_transforms();
			});
	}

	ImGui::End();
}

bool InstancesInspector::render_instance_transform(Transform& transform) {
	ImGui::Text("Transform");

	bool changed = false;

	float pos[3] = { transform.position.x, transform.position.y, transform.position.z };

	if (ImGui::Button("+X")) { pos[0] += 1.0f; changed = true; }
	ImGui::SameLine();
	if (ImGui::Button("+Y")) { pos[1] += 1.0f; changed = true; }
	ImGui::SameLine();
	if (ImGui::Button("+Z")) { pos[2] += 1.0f; changed = true; }

	if (ImGui::InputFloat3("Position", pos, "%.3f", ImGuiInputTextFlags_CharsScientific)) {
		changed = changed || glm::length((glm::vec3(pos[0], pos[1], pos[2]) - transform.position)) > 0.001f;
	}

	if (ImGui::Button("-X")) { pos[0] -= 1.0f; changed = true; }
	ImGui::SameLine();
	if (ImGui::Button("-Y")) { pos[1] -= 1.0f; changed = true; }
	ImGui::SameLine();
	if (ImGui::Button("-Z")) { pos[2] -= 1.0f; changed = true; }


	glm::vec3 pyr = glm::degrees(glm::eulerAngles(transform.rotation));

	float angles[3] = { pyr.x, pyr.y, pyr.z };
	if (ImGui::InputFloat3("Rotation", angles, "%.3f")) {
		changed = changed || glm::length((glm::vec3(angles[0], angles[1], angles[2]) - pyr)) > 0.001f;
	}

	float scale[3] = { transform.scale.x, transform.scale.y, transform.scale.z };
	if (ImGui::InputFloat3("Scale", scale, "%.3f")) {
		changed = changed || glm::length((glm::vec3(scale[0], scale[1], scale[2]) - glm::vec3(transform.scale))) > 0.001f;
	}

	if (changed) {
		transform.position[0] = pos[0];
		transform.position[1] = pos[1];
		transform.position[2] = pos[2];
		transform.scale[0] = scale[0];
		transform.scale[1] = scale[1];
		transform.scale[2] = scale[2];
		float pitch = glm::radians(angles[0]);
		float yaw = glm::radians(angles[1]);
		float roll = glm::radians(angles[2]);
		transform.rotation = glm::quat(glm::vec3(pitch, yaw, roll)); // Note: XYZ = pitch, yaw, roll
	}

	return changed;
}

}
