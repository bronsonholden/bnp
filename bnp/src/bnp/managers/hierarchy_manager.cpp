#include <bnp/managers/hierarchy_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/hierarchy.h>

namespace bnp {

// todo: method to create mutual parent/child components

void HierarchyManager::update(entt::registry& registry) {
	propagate_dirty_flag(registry);
	update_root_node_transforms(registry);
	update_hierarchical_transforms(registry);
}

void HierarchyManager::propagate_dirty_flag(entt::registry& registry) {
	int dirty_children;

	do {
		dirty_children = 0;
		auto view = registry.view<Transform, Parent>();

		for (auto entity : view) {
			auto& transform = view.get<Transform>(entity);
			auto& parent = view.get<Parent>(entity);

			// todo: prevent this
			if (entity == parent.value) continue;

			// if parent has no transform, do nothing
			if (registry.all_of<Transform>(parent.value)) {
				auto& parent_transform = registry.get<Transform>(parent.value);

				// don't update until parent is clean
				if (parent_transform.dirty && !transform.dirty) {
					registry.patch<Transform>(entity, [](Transform& t) {
						t.dirty = true;
						});
					++dirty_children;
				}
			}
		}
	} while (dirty_children > 0);
}

void HierarchyManager::update_root_node_transforms(entt::registry& registry) {
	auto view = registry.view<Transform>(entt::exclude<Parent>);
	int count = 0;

	for (auto entity : view) {
		++count;
		auto& transform = view.get<Transform>(entity);

		if (transform.dirty) {
			registry.patch<Transform>(entity, [](Transform& t) {
				t.update_world_transform(glm::mat4(1.0f));
				});
		}
	}
}

void HierarchyManager::update_hierarchical_transforms(entt::registry& registry) {
	int dirty_parents;

	do {
		dirty_parents = 0;
		auto view = registry.view<Transform, Parent>();

		for (auto entity : view) {
			auto& transform = view.get<Transform>(entity);
			auto& parent = view.get<Parent>(entity);

			// todo: prevent this
			if (entity == parent.value) continue;

			glm::mat4 parent_world_transform(1.0f);

			// if parent has no transform, treat it as a root node.
			// weird, may change to require transform for hierarchies
			if (registry.all_of<Transform>(parent.value)) {
				auto& parent_transform = registry.get<Transform>(parent.value);

				// don't update until parent is clean
				if (parent_transform.dirty) {
					++dirty_parents;
					continue;
				}

				parent_world_transform = parent_transform.world_transform;
			}

			if (transform.dirty) {
				registry.patch<Transform>(entity, [&](Transform& t) {
					t.update_world_transform(parent_world_transform);
					});
			}
		}
	} while (dirty_parents > 0);
}

}
