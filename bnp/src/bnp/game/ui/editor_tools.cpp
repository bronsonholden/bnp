#include <bnp/game/ui/editor_tools.h>

#include <imgui.h>

namespace bnp {
namespace Game {
namespace UI {

void EditorTools::initialize(entt::registry& registry) {
	universe_editor.initialize(registry);
	chemicals_editor.initialize(registry);
	blueprints_editor.initialize(registry);
	recipes_editor.initialize(registry);
}

void EditorTools::render(entt::registry& registry) {
	ImGui::Begin("Editor tools");

	if (ImGui::BeginTabBar("Tools")) {
		if (ImGui::BeginTabItem("Navigation Control")) {
			navigation_control.render(registry);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Blueprints editor")) {
			blueprints_editor.render(registry);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Universe editor")) {
			universe_editor.render(registry);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Items editor")) {
			items_editor.render(registry);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Chemicals editor")) {
			chemicals_editor.render(registry);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Recipes editor")) {
			recipes_editor.render(registry);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void EditorTools::update(entt::registry& registry, float dt) {

}

}
}
}
