#include <bnp/game/prefabs/galaxy.h>
#include <bnp/game/components/universe.h>

namespace bnp {
namespace Game {
namespace Prefab {

// todo: maybe convert model into a single entity with vectors/maps for systems and celestials
void Galaxy::model(entt::registry& registry) {
	Node galaxy(registry);
	galaxy.add_component<Game::Component::Galaxy>(
		100000.0
	);

	Node blochfeld(registry);
	blochfeld.add_component<Game::Component::System>(
		Game::Component::System{
			.id = 1,
			.name = "Blochfeld",
			.galaxy_position = glm::vec2(-1.54f, 0.926f),
			.light_minute_scale = 532.0
		}
	);

	Node eden(registry);
	eden.add_component<Game::Component::Celestial>(
		Game::Component::Celestial{
			.id = 1,
			.system_id = 1,
			.name = "Eden",
			.orbit_radius = 2.0
		}
	);
}

}
}
}
