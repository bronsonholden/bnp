#include <bnp/game/prefabs/ui.h>
#include <bnp/factories/sprite_factory.h>
#include <bnp/components/ui.h>

namespace bnp {
namespace Game {
namespace Prefab {

Node UI::play_button(entt::registry& registry, ResourceManager& resource_manager) {
	std::filesystem::path sprite_path = "resources/sprites/ui/play_button.png";
	std::filesystem::path json_path = "resources/sprites/ui/play_button.json";
	Node node(registry);

	Texture texture = resource_manager.load_texture(sprite_path.string(), sprite_path);
	Material material = resource_manager.load_material("sprite_material", {
		{ShaderType::VertexShader, "resources/shaders/sprite_vertex_shader.glsl"},
		{ShaderType::FragmentShader, "resources/shaders/sprite_fragment_shader.glsl"}
		});

	node.add_component<Texture>(texture);
	node.add_component<Renderable>(true);
	node.add_component<Material>(material);
	node.add_component<Button>();
	node.add_component<Hoverable>();

	SpriteFactory sprite_factory;
	sprite_factory.load_from_aseprite(node, json_path);

	auto& sprite = node.get_component<Sprite>();

	float w = static_cast<float>(sprite.spritesheet_width) / 64.0f;
	float h = static_cast<float>(sprite.spritesheet_height) / 64.0f;

	node.add_component<Transform>(Transform{
		glm::vec3(0.0f),
		glm::quat(),
		glm::vec3(w, h, 0.0f)
		});

	return node;
}

}
}
}
