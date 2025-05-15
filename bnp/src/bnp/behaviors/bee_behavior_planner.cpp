#include <bnp/core/logger.hpp>
#include <bnp/components/physics.h>
#include <bnp/components/transform.h>
#include <bnp/components/hierarchy.h>
#include <bnp/behaviors/bee_behavior_planner.h>
#include <bnp/helpers/random_float_generator.hpp>

#include <glm/glm.hpp>
#include <iostream>
using namespace std;

namespace bnp {

	void BeeBehaviorPlanner::update(entt::registry& registry, float dt) {
		std::vector<entt::entity> bees = get_bees(registry);

		plan_goals(registry, bees, dt);
		execute_goals(registry, bees, dt);
	}

	void BeeBehaviorPlanner::plan_goals(entt::registry& registry, std::vector<entt::entity> bees, float dt) {
		for (auto bee : bees) {
			auto& brain = registry.get_or_emplace<BehaviorBrain>(bee, BehaviorBrain{});

			// first priority: flee from threats
			for (auto threat : get_threats(registry, bee)) {
				bool already_fleeing = false;

				// if not fleeing, add
				if (!std::count_if(brain.goals.begin(), brain.goals.end(), [&](const auto& goal) {
					return goal.type == BehaviorGoal::Type::Flee && goal.target == threat;
					}))
				{
					auto& body = registry.get<PhysicsBody2D>(threat);

					// todo: customize perception by requiring movement or not?
					// moving threats only
					//if (body.body->GetLinearVelocity().Length() < 0.001f) continue;

					RandomFloatGenerator rfg(0.3f, 1.0f);
					// todo: insert such that bee flees from highest-threat first
					brain.goals.insert(brain.goals.begin(), BehaviorGoal{
						BehaviorGoal::Type::Flee,
						1.0f,
						rfg.generate(),
						1.0f,
						threat,
						false
						});
				}

			}

			int fleeing = 0, expired_fleeing = 0, visiting = 0, expired_visiting = 0, wandering = 0, expired_idling = 0, any_idling = 0;

			for (auto& goal : brain.goals) {
				if (goal.type == goal.Flee && goal.motivation > 0.0f) fleeing++;
				if (goal.type == goal.Flee && goal.motivation <= 0.0f) expired_fleeing++;
				if (goal.type == goal.Visit && goal.motivation > 0.0f) visiting++;
				if (goal.type == goal.Visit && goal.motivation <= 0.0f) expired_visiting++;
				if (goal.type == goal.Wander && goal.motivation > 0.0f) wandering++;
				if (goal.type == goal.Idle && goal.motivation <= 0.0f) expired_idling++;
				if (goal.type == goal.Idle) any_idling++;
			}

			// if fleeing, no motivation to idle or visit
			if (fleeing > 0) {
				for (auto& goal : brain.goals) {
					if (goal.type == goal.Idle || goal.type == goal.Visit || goal.type == goal.Wander) {
						goal.motivation = -1;
					}
				}
			}
			else if (visiting > 0 || wandering > 0) {
				for (auto& goal : brain.goals) {
					if (goal.type == goal.Idle) {
						goal.motivation = -1;
					}
				}
			}
			else if (any_idling > 0 && any_idling == expired_idling) {
				if (registry.all_of<BehaviorNest>(bee) && !wandering) {
					auto& nest = registry.get<BehaviorNest>(bee);
					brain.goals.insert(brain.goals.begin(), BehaviorGoal{
						BehaviorGoal::Wander,
						1.0f,
						2.0f,
						0.15f * 1.414f,
						nest.entity,
						false
						});
				}
				else {
					RandomFloatGenerator rfg(0.8f, 1.3f);
					// todo: dedup
					brain.goals.push_back(BehaviorGoal{
						BehaviorGoal::Type::Idle,
						1.0f,
						rfg.generate()
						});
				}
			}
			else if (any_idling == 0) {
				// after fleeing or visiting expires, idle with near-zero motivation so we go immediately into wandering
				float motivation = 0.001f;
				if (!expired_fleeing && !expired_visiting) {
					motivation = RandomFloatGenerator(0.2f, 0.7f).generate();
				}

				if (registry.all_of<BehaviorNest>(bee)) {
					auto& nest = registry.get<BehaviorNest>(bee);
					auto& nest_transform = registry.get<Transform>(nest.entity);
					nest_transform.update_world_transform(glm::mat4(1.0f));
					glm::vec2 nest_position = nest_transform.world_transform[3];
					auto& transform = registry.get<Transform>(bee);
					glm::vec2 bee_position = transform.world_transform[3];

					if (nest.min_return_distance < glm::length(nest_position - bee_position)) {
						auto& field = registry.get_or_emplace<FlowField2D>(nest.entity, FlowField2D{
							0.25f,
							{ 60, 60 },
							{ 0, 0 }
							});

						if (!field.init) {
							field.reposition_to_target(nest_position);
							field.generate_cost_field(registry);
							field.generate_direction_field(registry);
						}

						// random radius and motivation for visit to nest
						RandomFloatGenerator radius_rfg(0.5f, 1.1f);
						RandomFloatGenerator motivation_rfg(0.3f, 2.0f);

						brain.goals.insert(brain.goals.begin(), BehaviorGoal{
							BehaviorGoal::Visit,
							1.0f,
							motivation_rfg.generate(),
							radius_rfg.generate(),
							nest.entity,
							false
							});
					}
					else {
						// todo: dedup
						brain.goals.push_back(BehaviorGoal{
							BehaviorGoal::Type::Idle,
							1.0f,
							motivation
							});
					}

				}
				else {
					// todo: dedup
					brain.goals.push_back(BehaviorGoal{
						BehaviorGoal::Type::Idle,
						1.0f,
						motivation
						});
				}

			}

			// todo: swarming behavior idea. add new defend behavior that is prioritized above
			// flee. this behavior will be spread to nearby bees after a certain amount of time

			// now cleanup any unmotivated goals, first removing any transient targets
			for (auto& goal : brain.goals) {
				if (goal.motivation > 0.0f) continue;

				if (goal.transient && (int)goal.target) {
					registry.destroy(goal.target);
				}
			}

			auto new_end = std::remove_if(brain.goals.begin(), brain.goals.end(), [](BehaviorGoal& goal) {
				return goal.motivation <= 0.0f;
				});

			if (new_end != brain.goals.end()) {
				brain.goals.erase(new_end, brain.goals.end());
			}
		}
	}

	void BeeBehaviorPlanner::execute_goals(entt::registry& registry, std::vector<entt::entity> bees, float dt) {
		for (auto bee : bees) {
			auto& brain = registry.get<BehaviorBrain>(bee);

			if (!brain.goals.size()) continue;

			auto& goal = brain.goals.front();

			if (goal.motivation <= 0.0f) continue;

			switch (goal.type) {
			case goal.Flee:
				execute_flee(registry, goal, bee);
				break;
			case goal.Idle:
				execute_idle(registry, goal, bee);
				break;
			case goal.Visit:
				execute_visit(registry, goal, bee);
				break;
			case goal.Wander:
				execute_wander(registry, goal, bee);
				break;
			default:
				;
			}
		}
	}

	void BeeBehaviorPlanner::execute_wander(entt::registry& registry, BehaviorGoal& goal, entt::entity bee) {
		if (!registry.all_of<Motility>(bee)) {
			return;
		}

		auto& transform = registry.get<Transform>(bee);
		auto& motility = registry.get<Motility>(bee);

		if (!registry.all_of<FlowField2D>(goal.target)) {
			Log::warning("Wander target (entity=%d) has no FlowField2D component", static_cast<int>(goal.target));
			return;
		}

		auto& field = registry.get<FlowField2D>(goal.target);

		// let field init before executing
		if (!field.init) {
			return;
		}

		glm::vec2 dir = field.sample_wander_direction(transform.world_transform[3]);

		// if can't get there, abort
		if (glm::length(dir) < 0.001f) {
			goal.motivation = -1;
			return;
		}

		registry.patch<Motility>(bee, [&](Motility& m) {
			m.impulse = glm::vec3(dir, 0);
			m.flying_response = 1.0f;
			});
	}

	// todo: querying against a flow field needs to go into a generic function
	void BeeBehaviorPlanner::execute_visit(entt::registry& registry, BehaviorGoal& goal, entt::entity bee) {
		if (!registry.all_of<Motility>(bee)) {
			return;
		}

		auto& transform = registry.get<Transform>(bee);
		auto& motility = registry.get<Motility>(bee);

		glm::vec2 position = transform.world_transform[3];
		glm::vec2 target_position = registry.get<Transform>(goal.target).world_transform[3];

		if (glm::length(target_position - position) < goal.radius) {
			goal.motivation = -1;
			return;
		}

		if (!registry.all_of<FlowField2D>(goal.target)) {
			return;
		}

		auto& field = registry.get<FlowField2D>(goal.target);

		// let field init before executing
		if (!field.init) {
			return;
		}

		glm::vec2 dir = field.sample_direction(transform.world_transform[3]);

		// if can't get there, abort
		if (glm::length(dir) < 0.001f) {
			goal.motivation = -1;
			return;
		}

		registry.patch<Motility>(bee, [&](Motility& m) {
			m.impulse = glm::vec3(dir, 0);
			m.flying_response = 1.0f;
			});
	}

	// todo: querying against a flow field needs to go into a generic function
	void BeeBehaviorPlanner::execute_flee(entt::registry& registry, BehaviorGoal& goal, entt::entity bee) {
		if (!registry.all_of<Motility>(bee)) {
			return;
		}

		auto& transform = registry.get<Transform>(bee);
		auto& motility = registry.get<Motility>(bee);

		if (!registry.all_of<FlowField2D>(goal.target)) {
			return;
		}

		auto& field = registry.get<FlowField2D>(goal.target);

		// let field init before executing
		if (!field.init) {
			return;
		}

		glm::vec2 dir = field.sample_reverse_direction(transform.world_transform[3]);

		// can't proceed to target, abandon visit
		if (glm::length(dir) < 0.001f) {
			goal.motivation = -1;
			return;
		}

		registry.patch<Motility>(bee, [&](Motility& m) {
			m.speed = 1.1f;
			m.impulse = glm::vec3(dir, 0);
			m.flying_response = 1.0f;
			});
	}

	void BeeBehaviorPlanner::execute_idle(entt::registry& registry, BehaviorGoal& goal, entt::entity bee) {
		registry.patch<Motility>(bee, [](Motility& m) {
			m.impulse = glm::vec3(0);
			m.speed = 0.4f;
			m.flying_response = 1.0f;
			});
	}

	std::vector<entt::entity> BeeBehaviorPlanner::get_bees(entt::registry& registry) {
		std::vector<entt::entity> bees;
		auto view = registry.view<Identity, Transform>();

		for (auto entity : view) {
			auto& identity = view.get<Identity>(entity);

			if (identity.species == "Bee") {
				bees.push_back(entity);
			}
		}

		return bees;
	}

	std::vector<entt::entity> BeeBehaviorPlanner::get_threats(entt::registry& registry, entt::entity bee) {
		std::vector<entt::entity> threats;
		auto potential_threats_view = registry.view<Perception, Transform>();

		auto& transform = registry.get<Transform>(bee);
		glm::vec2 bee_position = transform.world_transform[3];

		for (auto entity : potential_threats_view) {
			auto& profile = potential_threats_view.get<Perception>(entity);
			auto& threat_transform = potential_threats_view.get<Transform>(entity);

			if (profile.threat.find("Bee") == profile.threat.end()) continue;
			if (profile.threat.at("Bee") < 1) continue;

			glm::vec2 threat_position = threat_transform.world_transform[3];

			// how close to qualify as a threat
			if (glm::length(threat_position - bee_position) > 1.0f) continue;

			threats.push_back(entity);
		}

		return threats;
	}

}
