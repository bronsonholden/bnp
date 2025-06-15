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
	void save_to_file(entt::registry& registry);
};

}
}
}
