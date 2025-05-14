#include <bnp/managers/behavior_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/hierarchy.h>

#include <queue>
#include <glm/glm.hpp>

namespace bnp {

	BehaviorManager::BehaviorManager()
	{

	}

	BehaviorManager::~BehaviorManager() {

	}


	void BehaviorManager::update(entt::registry& registry, float dt) {
		tick_goals(registry, dt);
		update_targets(registry, dt);
		regenerate_if_stale(registry, dt);
	}

	void BehaviorManager::tick_goals(entt::registry& registry, float dt) {
		auto view = registry.view<BehaviorBrain>();

		for (auto entity : view) {
			auto& brain = view.get<BehaviorBrain>(entity);

			for (auto& goal : brain.goals) {
				goal.motivation -= dt * goal.decay;
				goal.elapsed_time += dt;
			}
		}
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


			registry.patch<FlowField2D>(entity, [&](FlowField2D& field) {
				if (regenerate) {
					field.generate_cost_field(registry);
					field.generate_direction_field(registry);
				}
				field.wander_time += dt;
				field.generate_wander_field();
				});

		}
	}

}
