#include <bnp/components/physics.h>
#include <bnp/components/transform.h>
#include <bnp/behaviors/bee_behavior_planner.h>

#include <glm/glm.hpp>
#include <iostream>
using namespace std;

namespace bnp {

	void BeeBehaviorPlanner::update(entt::registry& registry, float dt) {
		std::vector<entt::entity> bees = get_bees(registry);

		update_goals(registry, bees, dt);
		execute_goals(registry, bees, dt);
	}

	void BeeBehaviorPlanner::update_goals(entt::registry& registry, std::vector<entt::entity> bees, float dt) {
		for (auto bee : bees) {
			std::vector<entt::entity> threats = get_threats(registry, bee);
			auto& brain = registry.get_or_emplace<BehaviorBrain>(bee, BehaviorBrain{});

			registry.patch<BehaviorBrain>(bee, [&](BehaviorBrain& b) {
				// flee behavior
				for (auto threat : threats) {
					bool current = false;

					for (auto& goal : b.goals) {
						// keep fleeing from continuing threats
						if (goal.type == goal.Flee && goal.target == threat) {
							goal.motivation = 0.15f;
							current = true;
							break;
						}
					}

					// if already a threat, we don't need to add another goal
					if (current) continue;

					b.goals.push_back(BehaviorGoal{
						BehaviorGoal::Type::Flee,
						1.0f,
						0.5f,
						threat
						});
				}
				});
		}
	}

	void BeeBehaviorPlanner::execute_goals(entt::registry& registry, std::vector<entt::entity> bees, float dt) {
		for (auto bee : bees) {
			auto& brain = registry.get<BehaviorBrain>(bee);

			if (!brain.goals.size()) {
				// todo: add return to origin
				auto& body = registry.get<PhysicsBody2D>(bee);
				body.body->SetLinearVelocity(b2Vec2(0, 0));
				continue;
			}

			auto& goal = brain.goals.front();

			switch (goal.type) {
			case goal.Flee:
				execute_flee(registry, goal, bee);
				break;
			default:
				;
			}
		}
	}

	void BeeBehaviorPlanner::execute_flee(entt::registry& registry, const BehaviorGoal& goal, entt::entity bee) {
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
			m.impulse = glm::vec3(dir, 0);
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
