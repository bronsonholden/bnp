#pragma once

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace UI {

class UniverseEditor {
public:
	UniverseEditor() = default;
	void render(entt::registry& registry);
};

}
}
}
