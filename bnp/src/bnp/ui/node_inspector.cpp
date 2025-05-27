#include <bnp/ui/node_inspector.h>
#include <bnp/ui/components/transform_inspector.h>
#include <bnp/ui/components/instances_inspector.h>
#include <bnp/ui/components/sprite_inspector.h>
#include <bnp/ui/components/behavior_brain_inspector.h>
#include <bnp/components/physics.h>
#include <bnp/components/behavior.h>

namespace bnp {

	NodeInspector::NodeInspector(Node node)
		: node(node) {}

	void NodeInspector::render() {
		if (!node.valid()) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Invalid node.");
			return;
		}

		ImGui::Begin("Node Inspector");

		ImGui::Text("Entity ID: %u", static_cast<uint32_t>(node.get_entity_id()));
		ImGui::Separator();

		ImGui::Text("Components:");
		ImGui::Indent();

		// This assumes you have ALL_COMPONENTS defined somewhere
		// For now, we'll manually check known types — you can template this later
		if (node.has_component<Transform>()) {
			if (ImGui::TreeNode("Transform")) {
				TransformInspector transform_inspector(node);

				transform_inspector.render();
				// TODO: Add editable fields
				ImGui::TreePop();
			}
		}

		if (node.has_component<Parent>()) {
			if (ImGui::TreeNode("Parent")) {
				ImGui::Text("Component: Parent");
				// TODO: Show parent entity ID
				ImGui::TreePop();
			}
		}

		if (node.has_component<Instances>()) {
			if (ImGui::TreeNode("Instances")) {
				InstancesInspector instances_inspector(node);

				instances_inspector.render();
				ImGui::TreePop();
			}
		}

		if (node.has_component<Sprite>()) {
			if (ImGui::TreeNode("Sprite")) {
				SpriteInspector sprite_inspector(node);

				sprite_inspector.render();
				ImGui::TreePop();
			}
		}

		if (node.has_component<Texture>()) {
			if (ImGui::TreeNode("Texture")) {
				SpriteInspector sprite_inspector(node);

				sprite_inspector.render();
				ImGui::TreePop();
			}
		}

		if (node.has_component<Material>()) {
			if (ImGui::TreeNode("Material")) {
				ImGui::TreePop();
			}
		}

		if (node.has_component<PhysicsBody2D>()) {
			if (ImGui::TreeNode("PhysicsBody2D")) {
				auto& body = node.get_component<PhysicsBody2D>();
				b2Vec2 position = body.body->GetTransform().p;
				float pos[2] = { position.x, position.y };
				ImGui::InputFloat2("Position", pos, "%.3f", ImGuiInputTextFlags_CharsScientific);
				ImGui::TreePop();
			}
		}

		if (node.has_component<BehaviorBrain>()) {
			if (ImGui::TreeNode("BehaviorBrain")) {
				BehaviorBrainInspector brain_inspector(node);

				brain_inspector.render();
				ImGui::TreePop();
			}
		}

		if (node.has_component<FlowField2D>()) {
			if (ImGui::TreeNode("FlowField2D")) {
				ImGui::TreePop();
			}
		}

		// Add more components here...
		// if (node.has_component<YourComponent>()) { ... }

		ImGui::Unindent();

		ImGui::End();
	}

}
