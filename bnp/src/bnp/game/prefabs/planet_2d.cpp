#include <bnp/game/prefabs/planet_2d.h>
#include <bnp/components/global.h>
#include <bnp/components/behavior.h>
#include <bnp/components/physics.h>
#include <bnp/components/controllable.h>
#include <bnp/components/world.h>

namespace bnp {

	Node Prefab::planet_2d(entt::registry& registry, ResourceManager& resource_manager) {
		Node node(registry);

		Material material = resource_manager.load_material("planet_material", {
			{ShaderType::VertexShader, "resources/shaders/planet_vertex_shader.glsl"},
			{ShaderType::FragmentShader, "resources/shaders/planet_fragment_shader.glsl"}
			});
		node.add_component<Renderable>(true);
		node.add_component<Material>(material);
		node.add_component<Planet2D>(
			0.0f,
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
			glm::vec3(0.3f, 0.36f, 0.2f)
		);

		node.add_component<Transform>(Transform{
			glm::vec3(2, 2, 0),
			glm::quat(), //glm::angleAxis(glm::radians(32.0f), glm::vec3(0,0, 1)),
			glm::vec3(2.0f)
			});

		node.add_component<Camera2DRig>(Camera2DRig{
			{ 0.0f, 0.0f }
			});

		return node;
	}

}
