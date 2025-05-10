#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace bnp {

	// todo: allow target to be a set of cells
	struct FlowField2D {
		float cell_size;
		glm::ivec2 grid_size;
		// bottom-left corner of the first cell (bottom-left)
		glm::vec2 origin;
		// world-space position of cell containing target
		glm::vec2 target;

		bool attract = false;
		bool init = false;
		bool dirty = true;

		std::vector<float> cost_field;

		// direction vectors for attraction
		std::vector<glm::vec2> direction_field;
		// direction vectors for repulsion.
		// todo: might remove and just use steering to handle repulsion
		std::vector<glm::vec2> reverse_field;
		std::vector<glm::vec2> wander_field;

		float wander_time = 0.0f;

		glm::vec4 get_aabb();
		glm::vec2 sample_direction(glm::vec2 worldspace_position);
		glm::vec2 sample_reverse_direction(glm::vec2 worldspace_position);
		glm::vec2 sample_wander_direction(glm::vec2 worldspace_position);
		void reposition_to_target(glm::vec2 worldspace_target);
		bool get_cell_position(glm::ivec2& result, glm::vec2 worlspace_position);
		bool is_target_cell(glm::vec2 worldspace_position);
		void generate_cost_field(entt::registry& registry);
		void generate_direction_field(entt::registry& registry);
		void generate_wander_field(float frequency = 0.9f, float speed = 1.0f, float swirl_scale = 0.2f);
		glm::vec2 smooth_direction(
			int x, int y,
			const std::vector<float>& cost_field,
			glm::ivec2 grid_size,
			bool attract
		);
		glm::vec2 adjust_for_blocked_neighbors(
			int fx, int fy,
			glm::vec2 field_position,
			glm::vec2 dir
		);
	};

	struct BeeBehavior {
		// how close an enemy can be before fleeing
		float min_safe_distance;
		// default position of the bee
		glm::vec2 origin;
		// how far the bee can wander from the origin
		float wander_distance;
		// how long the bee will stay at its current wader target
		float idle_time_remaining;
		float idle_time_min;
		float idle_time_max;
		// where the bee intends to be, calculated randomly around the origin
		glm::vec2 wander_target;
	};

	struct Identity {
		std::string species;
	};

	// how other entities perceive this entity
	struct Perception {
		// species, threat level
		std::unordered_map<std::string, int> threat;
	};

	struct BehaviorGoal {
		enum Type {
			Idle,
			Flee,
			Follow,
			Visit,
			Wander
		} type;

		// todo: behavior decay
		// how much motivation decays per second
		float decay;
		// while > 0, this goal can be executed. when zero, goal is removed
		// and, if transient is true, target entity is destroyed.
		float motivation;
		// max distance to satisfy visit or flee, min distance to follow
		float radius;

		entt::entity target;
		// if true, `target` is destroyed when goal completes
		bool transient = false;

		// time spent in this behavior
		float elapsed_time = 0.0f;
	};

	struct BehaviorBrain {
		std::vector<BehaviorGoal> goals;
	};

	// Special type of node for nesting behaviors
	struct BehaviorNest {
		entt::entity entity;
		// when beyond the is distance, dwellers should return
		float min_return_distance = 1.0f;
	};

	// Tag for nodes which are transient in behavior implementations, mostly
	// for debugging (toggle visibility in inspectors, etc.)
	struct Transient {
		bool value;
	};

}
