#pragma once

#include <entt/entt.hpp>

namespace bnp {

// updates world transforms for all nodes with a `Transform`
// component. For nodes with a parent, these are updated in top-down
// fashion, i.e. only nodes with a parent with a clean transform
// will be updated. This is done repeatedly until all transforms
// are clean.
class HierarchyManager {
public:
	HierarchyManager() = default;

	void update(entt::registry& registry);

private:
	void propagate_dirty_flag(entt::registry& registry);
	void update_root_node_transforms(entt::registry& registry);
	void update_hierarchical_transforms(entt::registry& registry);
};

}
