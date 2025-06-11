#include <bnp/game/queries/chemicals.h>

namespace bnp {
namespace Game {
namespace Queries {

entt::entity get_chemical_by_id(entt::registry& registry, Component::Chemical::ID chemical_id) {
	auto chemicals = registry.view<Component::Chemical>();

	for (auto entity : chemicals) {
		auto& chemical = chemicals.get<Component::Chemical>(entity);
		if (chemical.id == chemical_id) {
			return entity;
		}
	}

	return entt::null;
}

}
}
}
