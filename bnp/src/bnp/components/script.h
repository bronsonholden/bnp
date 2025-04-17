#pragma once

#include <bnp/core/node.hpp>
#include <entt/entt.hpp>

extern "C" {
#include <lua.h>
}

namespace bnp {

	struct Script {
		lua_State* L;
		bool ok;
		entt::registry* registry;
		entt::entity entity;
	};

	// Lightweight userdata that wraps a node, used in Lua bindings
	struct ScriptBindingContext {
		entt::registry* registry;
		entt::entity entity;

		Node node() {
			return Node(*registry, entity);
		}
	};

}
