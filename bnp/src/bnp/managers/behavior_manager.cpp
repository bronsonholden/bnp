#include <bnp/managers/behavior_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/hierarchy.h>

#include <queue>
#include <glm/glm.hpp>

namespace bnp {

	BehaviorManager::BehaviorManager(PhysicsManager& _physics_manager)
		: physics_manager(_physics_manager)
	{

	}

	BehaviorManager::~BehaviorManager() {

	}


	void BehaviorManager::update(entt::registry& registry, float dt) {
		update_targets(registry, dt);
		regenerate_if_stale(registry, dt);
	}

	void BehaviorManager::update_targets(entt::registry& registry, float dt) {
		auto view = registry.view<FlowField2D, Transform>();

		for (auto& entity : view) {
			auto& field = view.get<FlowField2D>(entity);
			auto& transform = view.get<Transform>(entity);

			glm::vec4 position = transform.world_transform * glm::vec4(0, 0, 0, 1);

			field.reposition_to_target(glm::vec2(position.x, position.y));
		}
	}

	void BehaviorManager::regenerate_if_stale(entt::registry& registry, float dt) {
		auto view = registry.view<FlowField2D, Transform>();

		for (auto& entity : view) {
			auto& field = view.get<FlowField2D>(entity);
			bool regenerate = !field.init;

			// todo: check if water overlap, update if it has changed
			// do this by calculating which columns include water, then
			// stepping down (from the highest row which water touches).
			// if any of these water-occupied cells have a valid cost,
			// regenerate the field

			if (regenerate) {
				registry.patch<FlowField2D>(entity, [&](FlowField2D& field) {
					// todo: these should go in a utility fn so that they can be generated
					// on-demand elsewhere
					field.generate_cost_field(registry);
					field.generate_direction_field(registry);
					});
			}
		}
	}

}
