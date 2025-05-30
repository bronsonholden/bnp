#include <bnp/game/prefabs/galaxy.h>
#include <bnp/game/components/universe.h>

namespace bnp {
namespace Game {

// todo: maybe convert model into a single entity with vectors/maps for systems and celestials
void Prefab::galaxy_model(entt::registry& registry) {
	Node galaxy(registry);
	galaxy.add_component<Game::Component::Galaxy>(
		100000.0
	);

	Node blochfeld(registry);
	blochfeld.add_component<Game::Component::System>(
		1,
		"Blochfeld",
		glm::vec2(-1.54f, 0.926f),
		532.0
	);

	Node eden(registry);
	eden.add_component<Game::Component::Celestial>(
		1,
		1,
		"Eden",
		2.0,
		0.0
	);
}

}
}
