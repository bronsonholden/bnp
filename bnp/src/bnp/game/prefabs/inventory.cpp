#include <bnp/game/prefabs/inventory.h>
#include <bnp/components/inventory.h>

namespace bnp {
namespace Prefab {

Node Inventory::item_database(entt::registry& registry) {
	Node node(registry);

	node.add_component<ItemDatabase>(ItemDatabase{
		{
			{"Iron Ore"}
		}
		});

	return node;
}

}
}
