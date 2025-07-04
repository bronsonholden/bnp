#include <bnp/core/logger.hpp>
#include <bnp/game/ui/navigation_control.h>
#include <bnp/game/managers/navigation_manager.h>
#include <bnp/serializers/registry.hpp>

#include <imgui.h>
#include <fstream>

#include <bnp/helpers/filesystem_helper.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>

namespace bnp {
namespace Game {
namespace UI {

void NavigationControl::render(entt::registry& registry) {
	if (ImGui::Button("Galaxy Map")) {
		Game::Manager::NavigationManager navigation_manager;
		navigation_manager.hide_galaxy_map(registry);
		navigation_manager.hide_celestial_map(registry);
		navigation_manager.hide_system_map(registry);
		navigation_manager.show_galaxy_map(registry);
	}
}

}
}
}
