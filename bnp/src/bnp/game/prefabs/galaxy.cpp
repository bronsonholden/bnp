#include <bnp/components/world.h>
#include <bnp/game/prefabs/galaxy.h>
#include <bnp/game/components/universe.h>

namespace bnp {
namespace Game {
namespace Prefab {

// todo: maybe convert model into a single entity with vectors/maps for systems and celestials
// todo: add way to save/load from file(s)
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

	Node maltonna(registry);
	maltonna.add_component<Game::Component::Celestial>(
		Game::Component::Celestial{
			.id = 2,
			.system_id = 1,
			.name = "Maltonna",
			.orbit_radius = 3.0
		}
	);
	maltonna.add_component<Planet2D>(
		Planet2D{
			.time = 0.0f,
			.rotation_speed = 0.035f,
			.noise_radius = 6.345f,
			.noise_seed = 23.77f,
			.axis = glm::vec3(-0.4f, 1.0f, 0.8f),
			.sun_direction = glm::vec3(0.4f, 0.6f, 0.54f),
			.water_depth = -0.3f,
			.coast_depth = 0.07f,
			.mainland_depth = 0.63f,
			.water_color = glm::vec3(0.34f, 0.19f, 0.16f),
			.coast_color = glm::vec3(0.25f, 0.16f, 0.23f),
			.mainland_color = glm::vec3(0.3f, 0.25f, 0.18f),
			.mountain_color = glm::vec3(0.42f, 0.29f, 0.22f),
			.cloud_depth = -100.0f,
			.cloud_radius = 5.0f,
			.cloud_radius_equator_exp = 0.4f,
			.cloud_banding_equator_exp = 1.7f,
			.crater_step = 1000.0f,
			.num_craters = 12
		}
	);
}

}
}
}
