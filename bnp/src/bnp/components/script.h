#pragma once

#include <bnp/core/node.hpp>
#include <entt/entt.hpp>

extern "C" {
#include <lua.h>
}

namespace bnp {

	struct Script {
		lua_State* L;
		entt::registry* registry;
		entt::entity entity;

		Node node() {
			return Node(*registry, entity);
		}
	};

}
