#pragma once

#include <bnp/game/components/recipes.h>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Queries {

entt::entity get_chemical_recipe_by_id(entt::registry& registry, Component::ChemicalRecipe::ID recipe_id);

}
}
}
