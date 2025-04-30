#include <bnp/components/physics.h>
#include <bnp/components/transform.h>
#include <bnp/components/hierarchy.h>
#include <bnp/behaviors/bee_behavior_planner.h>
#include <bnp/helpers/random_float_generator.hpp>

#include <glm/glm.hpp>
#include <iostream>
using namespace std;

namespace bnp {

	// todo: need a generic way to declare a 

	void BeeBehaviorPlanner::update(entt::registry& registry, float dt) {
		std::vector<entt::entity> bees = get_bees(registry);

		tick_goals(registry, bees, dt);
		plan_goals(registry, bees, dt);
		execute_goals(registry, bees, dt);
	}

	void BeeBehaviorPlanner::tick_goals(entt::registry& registry, std::vector<entt::entity> bees, float dt) {
		for (auto bee : bees) {
			auto& brain = registry.get_or_emplace<BehaviorBrain>(bee, BehaviorBrain{});

			for (auto& goal : brain.goals) {
				goal.motivation -= dt * goal.decay;
				goal.elapsed_time += dt;
			}
		}
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
					// todo: insert such that bee flees from highest-threat first
					brain.goals.insert(brain.goals.begin(), BehaviorGoal{
						BehaviorGoal::Type::Flee,
						1.0f,
						0.5f,
						1.0f,
						threat,
						false
						});
				}

			}

			// todo: maybe make this a single loop and increment counters?
			int fleeing = std::count_if(brain.goals.begin(), brain.goals.end(), [](const auto& goal) {
				return goal.type == goal.Flee && goal.motivation > 0.0f;
				});

			int expired_fleeing = std::count_if(brain.goals.begin(), brain.goals.end(), [](const auto& goal) {
				return goal.type == goal.Flee && goal.motivation <= 0.0f;
				});

			int visiting = std::count_if(brain.goals.begin(), brain.goals.end(), [](const auto& goal) {
				return goal.type == goal.Visit && goal.motivation > 0.0f;
				});

			int expired_idling = std::count_if(brain.goals.begin(), brain.goals.end(), [](const auto& goal) {
				return goal.type == goal.Idle && goal.motivation <= 0.0f;
				});

			int any_idling = std::count_if(brain.goals.begin(), brain.goals.end(), [](const auto& goal) {
				return goal.type == goal.Idle;
				});

			// if fleeing, no motivation to idle or visit
			if (fleeing > 0) {
				for (auto& goal : brain.goals) {
					if (goal.type == goal.Idle || goal.type == goal.Visit) {
						goal.motivation = -1;
					}
				}
			}
			else if (visiting > 0) {
				for (auto& goal : brain.goals) {
					if (goal.type == goal.Idle) {
						goal.motivation = -1;
					}
				}
			}
			else if (any_idling > 0 && any_idling == expired_idling) {
				RandomFloatGenerator rfg(-0.2, 0.2);
				entt::entity waypoint = registry.create();
				glm::vec2 bee_position = registry.get<Transform>(bee).world_transform[3];
				float x = rfg.generate();
				float y = rfg.generate();
				glm::vec2 offset(
					std::floor((x > 0 ? x + 0.25f : x - 0.25f) / 0.1f) * 0.1f,
					std::floor((y > 0 ? y + 0.1f : y - 0.1f) / 0.1f) * 0.1f
				);
				registry.emplace<Transform>(waypoint, Transform{
					glm::vec3(bee_position + offset, 0.0f)
					});
				registry.emplace<FlowField2D>(waypoint, FlowField2D{
					0.15f,
					{ 12, 12 },
					{ 0, 0 }
					});
				brain.goals.insert(brain.goals.begin(), BehaviorGoal{
					BehaviorGoal::Visit,
					1.0f,
					3.0f,
					0.15f * 1.414f,
					waypoint,
					true
					});
			}
			else if (any_idling == 0) {
				// after fleeing expires, idle with near-zero motivation so we go immediately into wandering
				float motivation = 0.001f;
				if (!expired_fleeing) {
					motivation = RandomFloatGenerator(0.2f, 2.3f).generate();
				}

				// todo: if too far from nest, push another visitor but biased towards nest.
				// use a high-resolution flow field and bias the random movement along
				// that (mostly static) field weighted by distance.
				//
				// ... code here ...
				//

				brain.goals.push_back(BehaviorGoal{
					BehaviorGoal::Type::Idle,
					1.0f,
					motivation
					});
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
			default:
				;
			}
		}
	}

	// todo: querying against a flow field needs to go into a generic function
	void BeeBehaviorPlanner::execute_visit(entt::registry& registry, BehaviorGoal& goal, entt::entity bee) {
		if (!registry.all_of<Motility>(bee)) {
			return;
		}

		auto& transform = registry.get<Transform>(bee);
		auto& motility = registry.get<Motility>(bee);

		if (!registry.all_of<FlowField2D>(goal.target)) {
			cout << "target has no flow field" << endl;
			return;
		}

		auto& field = registry.get<FlowField2D>(goal.target);

		if (!field.init) return;

		auto& target_transform = registry.get<Transform>(goal.target);
		auto& bee_transform = registry.get<Transform>(bee);
		glm::vec2 bee_pos = bee_transform.world_transform[3];
		glm::vec2 target_pos = target_transform.world_transform[3];

		if (glm::length(bee_pos - target_pos) < goal.radius) {
			goal.motivation = -1;
		}

		glm::vec2 field_position = glm::vec2(transform.position) - field.origin;

		int fx = std::floor(field_position.x / field.cell_size);
		int fy = std::floor(field_position.y / field.cell_size);

		if (fx < 0 || fx >= field.grid_size.x || fy < 0 || fy >= field.grid_size.y) {
			return;
		}

		int idx = fy * field.grid_size.x + fx;
		glm::vec2 dir = field.direction_field.at(idx);
		float cost = field.cost_field.at(idx);
		bool blocked_left = false, blocked_right = false, blocked_up = false, blocked_down = false;

		int tx = field.grid_size.x / 2;
		int ty = field.grid_size.y / 2;

		float target_cost = field.cost_field.at(ty * field.grid_size.x + tx);

		// if can't get there, abort
		if (target_cost == std::numeric_limits<float>::infinity()) {
			goal.motivation = -1;
			return;
		}

		if (cost == std::numeric_limits<float>::infinity() || glm::length(dir) < 0.001f) {
			const glm::ivec2 offsets[] = {
				{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
				{ 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
			};

			glm::vec2 best_dir(0);
			float best_cost = -std::numeric_limits<float>::infinity();

			for (auto& offset : offsets) {
				int ox = fx + offset.x;
				int oy = fy + offset.y;

				if (ox < 0 || ox >= field.grid_size.x || oy < 0 || oy >= field.grid_size.y) {
					continue;
				}

				int idx = oy * field.grid_size.x + ox;
				glm::vec2 neighbor_dir = field.reverse_field.at(idx);
				float cost = field.cost_field.at(idx);

				if (cost == std::numeric_limits<float>::infinity() || glm::length(neighbor_dir) < 0.001f) {
					continue;
				}

				if (cost > best_cost) {
					best_dir = glm::normalize(glm::vec2(offset.x, offset.y));
					best_cost = cost;
				}
			}

			dir = best_dir;
		}

		// calculate blocked cardinal cells to snap movement direction to avoid blocked cells
		const glm::ivec2 offsets[] = {
			{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
		};
		for (auto& offset : offsets) {
			int ox = fx + offset.x;
			int oy = fy + offset.y;

			if (ox < 0 || ox >= field.grid_size.x || oy < 0 || oy >= field.grid_size.y) {
				continue;
			}

			int idx = oy * field.grid_size.x + ox;
			float cost = field.cost_field.at(idx);

			if (cost == std::numeric_limits<float>::infinity()) {
				if (offset.x < 0) blocked_left = true;
				if (offset.x > 0) blocked_right = true;
				if (offset.y < 0) blocked_down = true;
				if (offset.y > 0) blocked_up = true;
				continue;
			}
		}

		glm::vec2 cell_position = field_position - glm::vec2(fx * field.cell_size, fy * field.cell_size);

		if (blocked_left && dir.x < 0 && (cell_position.x / field.cell_size) < 0.1f) dir.x = 0;
		if (blocked_right && dir.x > 0 && (cell_position.x / field.cell_size) > 0.9f) dir.x = 0;
		if (blocked_down && dir.y < 0 && (cell_position.y / field.cell_size) < 0.1f) dir.y = 0;
		if (blocked_up && dir.y > 0 && (cell_position.y / field.cell_size) > 0.9f) dir.y = 0;

		dir = glm::normalize(dir);

		registry.patch<Motility>(bee, [&](Motility& m) {
			m.impulse = glm::vec3(dir, 0);
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
			cout << "target has no flow field" << endl;
			return;
		}

		auto& field = registry.get<FlowField2D>(goal.target);

		if (!field.init) return;

		auto& target_transform = registry.get<Transform>(goal.target);
		auto& bee_transform = registry.get<Transform>(bee);
		glm::vec2 bee_pos = bee_transform.world_transform[3];
		glm::vec2 target_pos = target_transform.world_transform[3];

		if (glm::length(bee_pos - target_pos) <= goal.radius) {
			goal.motivation = 0.5f;
		}

		glm::vec2 field_position = glm::vec2(transform.position) - field.origin;

		int fx = std::floor(field_position.x / field.cell_size);
		int fy = std::floor(field_position.y / field.cell_size);

		if (fx < 0 || fx >= field.grid_size.x || fy < 0 || fy >= field.grid_size.y) {
			return;
		}

		int idx = fy * field.grid_size.x + fx;
		glm::vec2 dir = field.reverse_field.at(idx);
		float cost = field.cost_field.at(idx);
		bool blocked_left = false, blocked_right = false, blocked_up = false, blocked_down = false;

		if (cost == std::numeric_limits<float>::infinity() || glm::length(dir) < 0.001f) {
			const glm::ivec2 offsets[] = {
				{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
				{ 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
			};

			glm::vec2 best_dir(0);
			float best_cost = -std::numeric_limits<float>::infinity();

			for (auto& offset : offsets) {
				int ox = fx + offset.x;
				int oy = fy + offset.y;

				if (ox < 0 || ox >= field.grid_size.x || oy < 0 || oy >= field.grid_size.y) {
					continue;
				}

				int idx = oy * field.grid_size.x + ox;
				glm::vec2 neighbor_dir = field.reverse_field.at(idx);
				float cost = field.cost_field.at(idx);

				if (cost == std::numeric_limits<float>::infinity() || glm::length(neighbor_dir) < 0.001f) {
					continue;
				}

				if (cost > best_cost) {
					best_dir = glm::normalize(glm::vec2(offset.x, offset.y));
					best_cost = cost;
				}
			}

			dir = best_dir;
		}

		// calculate blocked cardinal cells to snap movement direction to avoid blocked cells
		const glm::ivec2 offsets[] = {
			{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
		};
		for (auto& offset : offsets) {
			int ox = fx + offset.x;
			int oy = fy + offset.y;

			if (ox < 0 || ox >= field.grid_size.x || oy < 0 || oy >= field.grid_size.y) {
				continue;
			}

			int idx = oy * field.grid_size.x + ox;
			float cost = field.cost_field.at(idx);

			if (cost == std::numeric_limits<float>::infinity()) {
				if (offset.x < 0) blocked_left = true;
				if (offset.x > 0) blocked_right = true;
				if (offset.y < 0) blocked_down = true;
				if (offset.y > 0) blocked_up = true;
				continue;
			}
		}

		auto& threat_transform = registry.get<Transform>(goal.target);
		glm::vec2 threat_position = threat_transform.world_transform[3];
		glm::vec2 bee_position = glm::vec2(transform.world_transform[3]);
		glm::vec2 threat_dir = threat_position - bee_position;
		dir = dir + (-threat_dir * 0.7f);

		glm::vec2 cell_position = field_position - glm::vec2(fx * field.cell_size, fy * field.cell_size);

		if (blocked_left && dir.x < 0 && (cell_position.x / field.cell_size) < 0.1f) dir.x = 0;
		if (blocked_right && dir.x > 0 && (cell_position.x / field.cell_size) > 0.9f) dir.x = 0;
		if (blocked_down && dir.y < 0 && (cell_position.y / field.cell_size) < 0.1f) dir.y = 0;
		if (blocked_up && dir.y > 0 && (cell_position.y / field.cell_size) > 0.9f) dir.y = 0;

		dir = glm::normalize(dir);

		registry.patch<Motility>(bee, [&](Motility& m) {
			m.speed = 1.1f;
			m.impulse = glm::vec3(dir, 0);
			m.flying_response = 0.02f;
			});
	}

	void BeeBehaviorPlanner::execute_idle(entt::registry& registry, BehaviorGoal& goal, entt::entity bee) {
		auto& body = registry.get<PhysicsBody2D>(bee);
		body.body->SetLinearVelocity(b2Vec2(0, 0));

		registry.patch<Motility>(bee, [](Motility& m) {
			m.speed = 0.4f;
			m.flying_response = 0.008f;
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
