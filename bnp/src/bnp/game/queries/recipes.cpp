#include <bnp/game/queries/recipes.h>

namespace bnp {
namespace Game {
namespace Queries {

entt::entity get_chemical_recipe_by_id(entt::registry& registry, Component::ChemicalRecipe::ID recipe_id) {
	auto recipes = registry.view<Component::ChemicalRecipe>();

	for (auto entity : recipes) {
		auto& recipe = recipes.get<Component::ChemicalRecipe>(entity);

		if (recipe.id == recipe_id) {
			return entity;
		}
	}

	return entt::null;
}

}
}
}
