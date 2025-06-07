#pragma once

#include <entt/entt.hpp>
#include <vector>

namespace bnp {
namespace Game {
namespace UI {

class ItemsEditor {
public:
	ItemsEditor() = default;

	void render(entt::registry& registry);

private:
	entt::entity editing_item_entity = entt::null;
	bool loaded = false;
	void save_to_file(entt::registry& registry);
	void load_from_file(entt::registry& registry);
	std::vector<entt::entity> get_item_entities_sorted_by_id(entt::registry& registry);
};

}
}
}
