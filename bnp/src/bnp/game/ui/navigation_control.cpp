#include <bnp/core/logger.hpp>
#include <bnp/game/ui/navigation_control.h>
#include <bnp/game/managers/navigation_manager.h>

#include <imgui.h>

namespace bnp {
namespace Game {

void NavigationControl::render(entt::registry& registry) {
	ImGui::Begin("Navigation Control");

	if (ImGui::Button("Galaxy Map")) {
		Game::Manager::NavigationManager navigation_manager;
		navigation_manager.hide_celestial_map(registry);
		navigation_manager.hide_system_map(registry);
		navigation_manager.show_galaxy_map(registry);
	}

	ImGui::End();
}

}
}
