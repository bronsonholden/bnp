#pragma once

#include <bnp/core/scene.h>

#include <optional>
#include <imgui.h>

namespace bnp {

class SceneInspector {
public:
	SceneInspector(entt::registry& registry);

	void render();

	// Optional: expose the selected entity
	std::optional<entt::entity> get_selected_entity() const;
	std::optional<entt::entity> get_inspected_entity() const;

private:
	entt::registry& registry;
	std::optional<entt::entity> selected_entity;
	std::optional<entt::entity> inspected_entity;
};

}
