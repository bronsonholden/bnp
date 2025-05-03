#include <bnp/ui/render_manager_inspector.h>

#include <imgui.h>

namespace bnp {

	RenderManagerInspector::RenderManagerInspector(RenderManager& _render_manager)
		: render_manager(_render_manager)
	{

	}

	void RenderManagerInspector::render() {
		ImGui::Begin("Debug Rendering");

		ImGui::Checkbox("Render wireframes", &render_manager.debug_render_wireframes);

		ImGui::End();
	}
}
