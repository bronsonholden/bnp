#include <bnp/game/components/inventory.h>
#include <bnp/game/queries/inventory.h>

namespace bnp {
namespace Game {
namespace Queries {

std::vector<entt::entity> get_item_entities_sorted_by_id(entt::registry& registry) {
	auto items = registry.view<Component::Item>();
	std::vector<entt::entity> entities;

	for (auto entity : items) {
		entities.push_back(entity);
	}

	std::sort(entities.begin(), entities.end(), [&registry](entt::entity a, entt::entity b) {
		auto& item_a = registry.get<Component::Item>(a);
		auto& item_b = registry.get<Component::Item>(b);
		return item_a.id < item_b.id;
		});

	return entities;
}

}
}
}
