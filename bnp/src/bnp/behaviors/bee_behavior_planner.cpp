#include <bnp/components/transform.h>
#include <bnp/behaviors/bee_behavior_planner.h>

#include <iostream>
using namespace std;

namespace bnp {

	void BeeBehaviorPlanner::update(entt::registry& registry, float dt) {
		std::vector<entt::entity> bees = get_bees(registry);

		for (auto bee : bees) {
			std::vector<entt::entity> threats = get_threats(registry, bee);
		}
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
