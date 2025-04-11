#pragma once

#include <bnp/core/scene.h>

#include <optional>
#include <imgui.h>

namespace bnp {

	class SceneInspector {
	public:
		SceneInspector(Scene& scene);

		void render();

		// Optional: expose the selected entity
		std::optional<entt::entity> get_selected_entity() const;
		std::optional<entt::entity> get_inspected_entity() const;

	private:
		Scene& scene;
		std::optional<entt::entity> selected_entity;
		std::optional<entt::entity> inspected_entity;
	};

}
