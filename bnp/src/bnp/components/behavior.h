#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace bnp {

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
			Flee,
			Follow
		} type;

		// todo: behavior decay
		// how much motivation decays per second
		float decay;
		// while > 0, this goal can be executed
		float motivation;

		entt::entity target;
	};

	struct BehaviorBrain {
		std::vector<BehaviorGoal> goals;
	};

}
