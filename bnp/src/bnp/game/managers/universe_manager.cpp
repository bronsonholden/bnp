#include <bnp/core/logger.hpp>
#include <bnp/game/managers/universe_manager.h>
#include <bnp/game/components/universe.h>

#include <numbers>

namespace bnp {
namespace Game {
namespace Manager {

void UniverseManager::update(entt::registry& registry, float dt) {
	auto view = registry.view<Game::Component::Universe>();
	auto count = view.size();

	if (count > 1) {
		Log::warning("Multiple Universe components found");
	}
	else if (!count) {
		Log::warning("No Universe component found");
		return;
	}

	registry.patch<Game::Component::Universe>(view.front(), [=](Game::Component::Universe& u) {
		u.time_elapsed += Game::Component::UNIVERSAL_TIME_SCALE * static_cast<double>(dt);
		});
}

}
}
}
