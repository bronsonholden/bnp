#include <bnp/game/prefabs/celestials.h>
#include <bnp/game/components/universe.h>
#include <bnp/game/components/extraction.h>
#include <bnp/game/components/navigation.h>
#include <bnp/components/global.h>
#include <bnp/components/behavior.h>
#include <bnp/components/physics.h>
#include <bnp/components/controllable.h>
#include <bnp/components/world.h>
#include <bnp/components/ui.h>
#include <bnp/factories/sprite_factory.h>

namespace bnp {
namespace Game {
namespace Prefab {

Node Celestials::celestial_orbit(entt::registry& registry, ResourceManager& resource_manager) {
	Node node(registry);

	Material material = resource_manager.load_material("system_orbit_material", {
		{ShaderType::VertexShader, "resources/shaders/system_orbit_vertex_shader.glsl"},
		{ShaderType::FragmentShader, "resources/shaders/system_orbit_fragment_shader.glsl"}
		});

	node.add_component<Material>(material);
	node.add_component<Renderable>(true);
	node.add_component<CirclePrimitive>();

	node.add_component<Transform>(Transform{
		glm::vec3(0, 0, 0),
		glm::angleAxis(0.0f, glm::vec3(0, 0, 1)),
		glm::vec3(4.0f, 4.0f, 1.0f)
		});

	return node;
}

Node Celestials::galaxy(entt::registry& registry, ResourceManager& resource_manager) {
	Node node(registry);

	Material material = resource_manager.load_material("galaxy_material", {
		{ShaderType::VertexShader, "resources/shaders/galaxy_vertex_shader.glsl"},
		{ShaderType::FragmentShader, "resources/shaders/galaxy_fragment_shader.glsl"}
		});

	node.add_component<Material>(material);
	node.add_component<Renderable>(true);

	// tag for updating renderable when viewing galaxy map
	node.add_component<Game::Component::GalaxyMap>(Game::Component::GalaxyMap{});

	// for rendering
	node.add_component<Galaxy2D>();

	node.add_component<Transform>(Transform{
		glm::vec3(0, 0, 0),
		glm::quat(),
		glm::vec3(6.0f)
		});

	node.add_component<Camera2DRig>(Camera2DRig{
		{ 0.0f, 0.0f }
		});

	return node;
}

Node Celestials::galaxy_map_dot(entt::registry& registry, ResourceManager& resource_manager) {
	std::filesystem::path sprite_path = "resources/sprites/galaxy_map/galaxy_map_dot/galaxy_map_dot.png";
	std::filesystem::path json_path = "resources/sprites/galaxy_map/galaxy_map_dot/galaxy_map_dot.json";
	Node node(registry);

	Material material = resource_manager.load_material("sprite_material", {
		{ShaderType::VertexShader, "resources/shaders/sprite_vertex_shader.glsl"},
		{ShaderType::FragmentShader, "resources/shaders/sprite_fragment_shader.glsl"}
		});

	Texture texture = resource_manager.load_texture(sprite_path.string(), sprite_path);
	node.add_component<Texture>(texture);
	node.add_component<Material>(material);
	node.add_component<Renderable>(true);

	node.add_component<Button>();
	node.add_component<Hoverable>();

	SpriteFactory sprite_factory;
	sprite_factory.load_from_aseprite(node, json_path);

	node.add_component<Transform>(Transform{
		glm::vec3(0),
		glm::quat(),
		glm::vec3(8.0f / 64.0f)
		});

	return node;
}

Node Celestials::celestial(entt::registry& registry, ResourceManager& resource_manager) {
	Node node(registry);

	Material material = resource_manager.load_material("planet_material", {
		{ShaderType::VertexShader, "resources/shaders/planet_vertex_shader.glsl"},
		{ShaderType::FragmentShader, "resources/shaders/planet_fragment_shader.glsl"}
		});

	node.add_component<Material>(material);
	node.add_component<Renderable>(true);

	node.add_component<Transform>(Transform{
		glm::vec3(0, 0, 0),
		glm::quat(),
		glm::vec3(2.5f)
		});

	node.add_component<Camera2DRig>(Camera2DRig{
		{ 0.0f, 0.0f }
		});

	node.add_component<Game::Component::CelestialExtractionTable>();

	return node;
}

}
}
}
