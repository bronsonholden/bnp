#pragma once

#include <bnp/components/behavior.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace bnp {

// todo: move flow field management to its own manager?
class BehaviorManager {
public:
	BehaviorManager();
	~BehaviorManager();

	void update(entt::registry& registry, float dt);

	void tick_goals(entt::registry& registry, float dt);
	void update_targets(entt::registry& registry, float dt);
	void regenerate_if_stale(entt::registry& registry, float dt);
};

}
