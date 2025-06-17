#pragma once

#include <bnp/ui/component_editor.hpp>
#include <bnp/game/ui/universe_editor.h>
#include <bnp/game/ui/navigation_control.h>
#include <bnp/game/ui/items_editor.h>
#include <bnp/game/ui/chemicals_editor.h>
#include <bnp/game/ui/blueprints_editor.h>
#include <bnp/game/ui/recipes_editor.h>
#include <bnp/game/components/factions.h>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace UI {

class EditorTools {
public:
	EditorTools() = default;

	void initialize(entt::registry& registry);
	void render(entt::registry& registry);
	void update(entt::registry& registry, float dt);

private:
	NavigationControl navigation_control;
	UniverseEditor universe_editor;
	ItemsEditor items_editor;
	ChemicalsEditor chemicals_editor;
	BlueprintsEditor blueprints_editor;
	RecipesEditor recipes_editor;
	ComponentEditor<Game::Component::Faction> faction_editor;
};

}
}
}
