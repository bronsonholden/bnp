#pragma once

#include <bnp/core/logger.hpp>
#include <bnp/game/components/recipes.h>

#include <bitsery/bitsery.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

namespace std {

template <typename S>
void serialize(S& s, pair<double, bnp::Game::Component::Chemical::ID> entry) {
	s.value8b(entry.first);
	s.value4b(entry.second);
	bnp::Log::info("chemical recipe entry %d, %2.2f", entry.first, entry.second);
}

}

namespace bnp {
namespace Game {
namespace Component {

template <typename S>
void serialize(S& s, Game::Component::ChemicalRecipe& recipe) {
	s.value4b(recipe.version);
	s.value4b(recipe.id);
	s.text1b(recipe.name, 256);
	s.container(recipe.inputs, 256, [](S& s, auto& pair) {
		s.value8b(pair.first);
		s.value4b(pair.second);
		});
	s.container(recipe.outputs, 256, [](S& s, auto& pair) {
		s.value8b(pair.first);
		s.value4b(pair.second);
		});
	s.value8b(recipe.energy);
	s.value8b(recipe.efficiency);
	s.value8b(recipe.protons_emitted);
	s.value8b(recipe.neutrons_emitted);

	recipe.version = recipe.latest_version;
}

}
}
}
