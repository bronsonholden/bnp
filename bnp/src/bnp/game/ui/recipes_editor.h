#pragma once

#include <entt/entt.hpp>
#include <filesystem>

namespace bnp {
namespace Game {
namespace UI {

class RecipesEditor {
public:
	RecipesEditor() = default;

	void initialize(entt::registry& registry);
	void render(entt::registry& registry);

private:
	entt::entity edit_chemical_recipe_entity = entt::null;
	void render_chemical_recipes_section(entt::registry& registry);
	void render_edit_chemical_recipe_section(entt::registry& registry);
	void save_to_file(entt::registry& registry);
};

}
}
}
