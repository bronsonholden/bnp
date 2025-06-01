#include <bnp/game/managers/universe_manager.h>
#include <bnp/game/components/universe.h>

#include <numbers>

namespace bnp {
namespace Game {
namespace Manager {

void UniverseManager::update(entt::registry& registry, float dt) {
	auto view = registry.view<Game::Component::Celestial>();

	for (auto entity : view) {
		auto& celestial = view.get<Game::Component::Celestial>(entity);

		registry.patch<Game::Component::Celestial>(entity, [=](Game::Component::Celestial& c) {
			double progression_step = (2.0 * std::numbers::pi) / c.orbit_duration;
			c.orbit_progression = std::fmod(
				c.orbit_progression + static_cast<double>(dt) * progression_step * Game::Component::UNIVERSAL_TIME_SCALE,
				std::numbers::pi * 2.0
			);

			double rotate_step = (2.0 * std::numbers::pi) / c.rotate_duration;
			c.rotate_progression = std::fmod(
				c.rotate_progression + static_cast<double>(dt) * rotate_step * Game::Component::UNIVERSAL_TIME_SCALE,
				std::numbers::pi * 2.0
			);
			});
	}
}

}
}
}
