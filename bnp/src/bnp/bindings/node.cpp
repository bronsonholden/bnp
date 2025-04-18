#include <bnp/bindings/node.h>
#include <bnp/components/physics.h>
#include <bnp/components/graphics.h>
#include <bnp/components/script.h>
#include <bnp/components/hierarchy.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace bnp {

	Node l_pop_script_node(lua_State* L, int index) {
		ScriptNode script_node = *static_cast<ScriptNode*>(lua_touserdata(L, index));

		return Node(script_node.registry, script_node.entity);
	}

	void l_push_script_node(lua_State* L, Node& node) {
		new (lua_newuserdata(L, sizeof(ScriptNode))) ScriptNode
		{
			node.get_registry(),
			node.get_entity_id()
		};
	}

	int l_node_add_child(lua_State* L) {
		Node node = l_pop_script_node(L);
		Node child(node.get_registry());

		child.add_component<Parent>(Parent{ node.get_entity_id() });
		l_push_script_node(L, child);

		return 1;
	}

	int l_node_add_component(lua_State* L) {
		Node node = l_pop_script_node(L, -2);

		const char* name = luaL_checkstring(L, 2); // colon syntax: arg 1 = node, arg 2 = name

		if (strcmp(name, "Sprite") == 0) {
			std::cout << "Adding Sprite to entity\n";
			node.add_component<Sprite>();
		}
		else if (strcmp(name, "Transform") == 0) {
			std::cout << "Adding Transform to entity\n";
			node.add_component<Transform>();
		}
		else if (strcmp(name, "PhysicsBody2D") == 0) {
			std::cout << "Adding PhysicsBody2D to entity\n";
			node.add_component<PhysicsBody2D>();
		}
		else {
			return luaL_error(L, "Unknown/disallowed component: %s", name);
		}

		l_push_script_node(L, node);

		return 1;
	}

	int l_node_get_component(lua_State* L) {
		Node node = l_pop_script_node(L, -2);

		const char* name = luaL_checkstring(L, 2); // colon syntax: arg 1 = node, arg 2 = name

		if (strcmp(name, "Sprite") == 0) {
			if (!node.has_component<Sprite>()) {
				lua_pushnil(L);
			}
			else {
				l_push_script_node(L, node);
				luaL_getmetatable(L, "bnp.Sprite");
				lua_setmetatable(L, -2);
			}

			return 1;
		}
		else if (strcmp(name, "Transform") == 0) {
			if (!node.has_component<Transform>()) {
				lua_pushnil(L);
			}
			else {
				l_push_script_node(L, node);
				luaL_getmetatable(L, "bnp.Transform");
				lua_setmetatable(L, -2);
			}

			return 1;
		}

		return luaL_error(L, "Unknown/disallowed component: %s", name);
	}

}
