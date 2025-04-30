#pragma once

#include <bnp/components/behavior.h>

#include <entt/entt.hpp>

namespace bnp {

	class BeeBehaviorPlanner {
	public:
		BeeBehaviorPlanner() = default;

		void update(entt::registry& registry, float dt);

	private:
		std::vector<entt::entity> get_bees(entt::registry& registry);
		std::vector<entt::entity> get_threats(entt::registry& registry, entt::entity bee);

		void plan_goals(entt::registry& registry, std::vector<entt::entity> bees, float dt);
		void tick_goals(entt::registry& registry, std::vector<entt::entity> bees, float dt);
		void execute_goals(entt::registry& registry, std::vector<entt::entity> bees, float dt);

		// old goal stuff - probably removing/refactoring
		//void clean_goals(entt::registry& registry, std::vector<entt::entity> bees, float dt);

		void execute_visit(entt::registry& registry, BehaviorGoal& goal, entt::entity bee);
		void execute_flee(entt::registry& registry, BehaviorGoal& goal, entt::entity bee);
		void execute_idle(entt::registry& registry, BehaviorGoal& goal, entt::entity bee);
	};

}
