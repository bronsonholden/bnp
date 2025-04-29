#include <bnp/ui/components/behavior_brain_inspector.h>

namespace bnp {

	BehaviorBrainInspector::BehaviorBrainInspector(Node& _node)
		: node(_node)
	{
	}

	const char* behavior_type_name(BehaviorGoal::Type type) {
		switch (type) {
		case BehaviorGoal::Flee:
			return "Flee";
		case BehaviorGoal::Follow:
			return "Follow";
		}
	}

	void BehaviorBrainInspector::render() {
		auto& brain = node.get_component<BehaviorBrain>();

		for (auto goal : brain.goals) {
			ImGui::Text(behavior_type_name(goal.type));
		}
	}

}
