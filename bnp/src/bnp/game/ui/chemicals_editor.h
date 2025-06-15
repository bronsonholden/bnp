#pragma once

#include <bnp/game/components/matter.h>

#include <entt/entt.hpp>
#include <vector>

namespace bnp {
namespace Game {
namespace UI {

class ChemicalsEditor {
public:
	ChemicalsEditor() = default;

	void initialize(entt::registry& registry);
	void render(entt::registry& registry);

private:
	Game::Component::Chemical::ID editing_chemical_id = 0;
	void save_to_file(entt::registry& registry);
	std::vector<entt::entity> get_chemical_entities_sorted_by_id(entt::registry& registry);
};

}
}
}
