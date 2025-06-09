#include <bnp/game/ui/editor_tools.h>

namespace bnp {
namespace Game {
namespace UI {

void EditorTools::update(entt::registry& registry, float dt) {

}

void EditorTools::render(entt::registry& registry) {
	universe_editor.render(registry);
	navigation_control.render(registry);
	items_editor.render(registry);
	chemicals_editor.render(registry);
}

}
}
}
