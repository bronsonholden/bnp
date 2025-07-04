#include <bnp/ui/components/sprite_inspector.h>

namespace bnp {

	SpriteInspector::SpriteInspector(Node& _node)
		: node(_node)
	{
	}

	void SpriteInspector::render() {
		auto& sprite = node.get_component<Sprite>();

		int z = sprite.z;

		ImGui::InputInt("Z-index", &z, 1, 10);

		node.get_registry().patch<Sprite>(node.get_entity_id(), [=](Sprite& s) {
			s.z = z;
			});
	}

}
