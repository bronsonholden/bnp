#include <bnp/components/world.h>
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
	eden.add_component<Planet2D>(
		Planet2D{
			.time = 0.0f,
			.rotation_speed = 0.07f,
			.noise_radius = 6.345f,
			.noise_seed = 1.14f,
			.axis = glm::vec3(0.4f, 1.0f, 0.3f),
			.sun_direction = glm::vec3(-0.4f, 0.2f, 1.1f),
			.water_depth = 0.08f,
			.coast_depth = 0.05f,
			.mainland_depth = 0.2f,
			.water_color = glm::vec3(0.1f, 0.15f, 0.76f),
			.coast_color = glm::vec3(0.15f, 0.2f, 0.5f),
			.mainland_color = glm::vec3(0.1f, 0.35f, 0.1f),
			.mountain_color = glm::vec3(0.3f, 0.36f, 0.2f),
			.cloud_depth = -0.125f
		}
	);
}

}
}
}
