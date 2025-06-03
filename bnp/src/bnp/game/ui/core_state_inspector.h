#pragma once

#include <entt/entt.hpp>

namespace bnp {

class CoreStateInspector {
public:
	CoreStateInspector() = default;
	void render(entt::registry& registry);
};

}
