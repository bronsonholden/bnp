#include <bnp/game/prefabs/squirrel.h>
#include <bnp/components/behavior.h>
#include <bnp/components/physics.h>
#include <bnp/components/controllable.h>
#include <bnp/components/world.h>
#include <bnp/factories/sprite_factory.h>

namespace bnp {

	Node Prefab::squirrel(entt::registry& registry, ResourceManager& resource_manager) {
		std::filesystem::path sprite_path = "resources/sprites/squirrel/squirrel.png";
		std::filesystem::path json_path = "resources/sprites/squirrel/squirrel.json";
		Node node(registry);

		Texture texture = resource_manager.load_texture(sprite_path.string(), sprite_path);
		Material material = resource_manager.load_material("sprite_material", {
			{ShaderType::VertexShader, "resources/shaders/sprite_vertex_shader.glsl"},
			{ShaderType::FragmentShader, "resources/shaders/sprite_fragment_shader.glsl"}
			});
		node.add_component<Texture>(texture);
		node.add_component<Renderable>(true);
		node.add_component<Material>(material);

		SpriteFactory sprite_factory;
		sprite_factory.load_from_aseprite(node, json_path);

		node.add_component<Transform>(Transform{
			glm::vec3(3, 3, 0),
			glm::quat(),
			glm::vec3(1)
			});
		node.add_component<Identity>(Identity{ "Squirrel" });
		node.add_component<Perception>(Perception{
			{
				{"Bee", 1}
			}
			});

		node.add_component<FlowField2D>(FlowField2D{
			0.25f,
			{ 40, 40 },
			{ 0, 0 }
			});

		node.add_component<Motility>(Motility{ 2.5f });
		node.add_component<Controllable>(Controllable{ true });

		node.add_component<Camera2DRig>(Camera2DRig{
			{ 0.0f, 1.0f }
			});

		node.add_component<SquirrelController>();
		node.add_component<KeyboardInputObserver>();
		node.add_component<KeyboardInputMapping>(KeyboardInputMapping{
			{
				{ SDL_SCANCODE_A, KeyboardInput::Action::MoveLeft },
				{ SDL_SCANCODE_D, KeyboardInput::Action::MoveRight },
				{ SDL_SCANCODE_W, KeyboardInput::Action::MoveUp },
				{ SDL_SCANCODE_S, KeyboardInput::Action::MoveDown },
				{ SDL_SCANCODE_SPACE, KeyboardInput::Action::Jump }
			}
			});

		return node;
	}

}
