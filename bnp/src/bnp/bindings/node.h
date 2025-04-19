#pragma once

#include <bnp/core/node.hpp>

extern "C" {
#include <lua.h>
}

namespace bnp {
	// Pops the `ScriptNode` from the top of the stack and returns a
	// constructed `Node` that can be used
	Node l_pop_script_node(lua_State* L, int index = -1);

	// Pushes a `ScriptNode` userdata onto the stack representing
	// the given `Node`
	void l_push_script_node(lua_State* L, Node& node);

	int l_node_create_child(lua_State* L);
	int l_node_add_component(lua_State* L);
	int l_node_get_component(lua_State* L);

}
