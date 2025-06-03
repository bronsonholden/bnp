#pragma once

#include <entt/entt.hpp>

namespace bnp {
namespace Game {

class NavigationControl {
public:
	NavigationControl() = default;
	void render(entt::registry& registry);
};

}
}
