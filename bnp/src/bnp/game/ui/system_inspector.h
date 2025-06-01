#pragma once

#include <bnp/core/node.hpp>

#include <entt/entt.hpp>
#include <imgui.h>
#include <string>

namespace bnp {
namespace Game {

class SystemInspector {
public:
	SystemInspector(entt::registry& registry);

	void render();

private:
	entt::entity system_entity;
	entt::registry& registry;
};

}
}
