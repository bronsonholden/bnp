#include <bnp/game/prefabs/celestials.h>
#include <bnp/components/game/universe.h>
#include <bnp/components/game/extraction.h>
#include <bnp/components/global.h>
#include <bnp/components/behavior.h>
#include <bnp/components/physics.h>
#include <bnp/components/controllable.h>
#include <bnp/components/world.h>

namespace bnp {
	namespace Prefab {

		Node Celestials::galaxy(entt::registry& registry, ResourceManager& resource_manager) {
			Node node(registry);

			Material material = resource_manager.load_material("planet_material", {
				{ShaderType::VertexShader, "resources/shaders/galaxy_vertex_shader.glsl"},
				{ShaderType::FragmentShader, "resources/shaders/galaxy_fragment_shader.glsl"}
				});

			node.add_component<Material>(material);
			node.add_component<Renderable>(true);

			node.add_component<Galaxy2D>(Galaxy2D{});

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

		Node Celestials::eden(entt::registry& registry, ResourceManager& resource_manager) {
			Node node(registry);

			Material material = resource_manager.load_material("planet_material", {
				{ShaderType::VertexShader, "resources/shaders/planet_vertex_shader.glsl"},
				{ShaderType::FragmentShader, "resources/shaders/planet_fragment_shader.glsl"}
				});

			node.add_component<Material>(material);
			node.add_component<Renderable>(true);

			node.add_component<Planet2D>(
				0.0f,
				0.07f,
				6.345f,
				1.14f,
				glm::vec3(0.4f, 1.0f, 0.3f),
				glm::vec3(-0.4f, 0.2f, 1.1f),
				0.1f,
				0.05f,
				0.2f,
				glm::vec3(0.1f, 0.15f, 0.76f),
				glm::vec3(0.15f, 0.2f, 0.5f),
				glm::vec3(0.1f, 0.35f, 0.1f),
				glm::vec3(0.3f, 0.36f, 0.2f),
				-0.325f
			);

			node.add_component<Transform>(Transform{
				glm::vec3(0, 0, 0),
				glm::quat(),
				glm::vec3(2.0f)
				});

			node.add_component<Camera2DRig>(Camera2DRig{
				{ 0.0f, 0.0f }
				});

			node.add_component<Celestial>(
				"Eden"
			);

			node.add_component<CelestialExtractionTable>();

			return node;
		}

	}
}
