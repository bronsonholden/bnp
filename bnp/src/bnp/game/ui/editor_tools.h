#pragma once

#include <bnp/game/ui/universe_editor.h>
#include <bnp/game/ui/navigation_control.h>
#include <bnp/game/ui/items_editor.h>
#include <bnp/game/ui/chemicals_editor.h>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace UI {

class EditorTools {
public:
	EditorTools() = default;

	void update(entt::registry& registry, float dt);
	void render(entt::registry& registry);

private:
	NavigationControl navigation_control;
	UniverseEditor universe_editor;
	ItemsEditor items_editor;
	ChemicalsEditor chemicals_editor;
};

}
}
}
