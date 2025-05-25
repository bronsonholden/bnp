#include <bnp/game/prefabs/cloaked_figure.h>
#include <bnp/components/global.h>
#include <bnp/components/behavior.h>
#include <bnp/components/physics.h>
#include <bnp/components/controllable.h>
#include <bnp/components/world.h>
#include <bnp/factories/sprite_factory.h>

namespace bnp {

	Node Prefab::cloaked_figure(entt::registry& registry, ResourceManager& resource_manager) {
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
			0.1f,
			0.05f,
			0.15f,
			glm::vec3(0.1f, 0.15f, 0.76f),
			glm::vec3(0.15f, 0.2f, 0.5f),
			glm::vec3(0.1f, 0.35f, 0.1f),
			glm::vec3(0.25f, 0.36f, 0.14f)
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
