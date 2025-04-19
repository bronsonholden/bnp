#include <bnp/bindings/node.h>
#include <bnp/components/physics.h>
#include <bnp/components/graphics.h>
#include <bnp/components/script.h>
#include <bnp/components/hierarchy.h>
#include <bnp/factories/sprite_factory.h>

#include <filesystem>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace bnp {

	Node l_pop_script_node(lua_State* L, int index) {
		ScriptNode script_node = *static_cast<ScriptNode*>(lua_touserdata(L, index));

		lua_remove(L, index);

		return Node(script_node.registry, script_node.entity);
	}

	void l_push_script_node(lua_State* L, Node& node) {
		new (lua_newuserdata(L, sizeof(ScriptNode))) ScriptNode
		{
			node.get_registry(),
			node.get_entity_id()
		};
	}

	int l_node_create_child(lua_State* L) {
		Node node = l_pop_script_node(L);
		Node child(node.get_registry());

		child.add_component<Parent>(Parent{ node.get_entity_id() });

		l_push_script_node(L, child);
		luaL_getmetatable(L, "bnp.Node");
		lua_setmetatable(L, -2);

		return 1;
	}

	int l_node_add_component_sprite(lua_State* L) {
		// [node, "Sprite", params]
		Node node = l_pop_script_node(L, 1);

		lua_getfield(L, 3, "spritesheet");

		SpriteFactory sprite_factory;
		sprite_factory.load_from_aseprite(node, lua_tostring(L, -1));
		lua_pop(L, 3);

		l_push_script_node(L, node);
		luaL_getmetatable(L, "bnp.Sprite");
		lua_setmetatable(L, -2);

		return 1;
	}

	int l_node_add_component_transform(lua_State* L) {
		// [node, "Transform", params]
		Node node = l_pop_script_node(L, 1);
		// ["Transform", params]

		glm::vec3 position(0);

		if (lua_istable(L, -1)) {
			lua_getfield(L, -1, "x");
			if (lua_isnumber(L, -1)) position.x = lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, -1, "y");
			if (lua_isnumber(L, -1)) position.y = lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, -1, "z");
			if (lua_isnumber(L, -1)) position.z = lua_tonumber(L, -1);
			lua_pop(L, 1);

			lua_pop(L, 1);
		}

		lua_pop(L, 1);
		// []

		node.add_component<Transform>(Transform{ position });

		l_push_script_node(L, node);
		luaL_getmetatable(L, "bnp.Transform");
		lua_setmetatable(L, -2);

		return 1;
	}

	int l_node_add_component(lua_State* L) {
		// [node, component_name, ...]
		const char* name = luaL_checkstring(L, 2);

		if (strcmp(name, "Sprite") == 0) {
			return l_node_add_component_sprite(L);
		}
		else if (strcmp(name, "Transform") == 0) {
			return l_node_add_component_transform(L);
		}
		else if (strcmp(name, "PhysicsBody2D") == 0) {
			std::cout << "Adding PhysicsBody2D to entity\n";
		}
		else {
			return luaL_error(L, "Unknown/disallowed component: %s", name);
		}
	}

	int l_node_get_component(lua_State* L) {
		Node node = l_pop_script_node(L, -2);

		const char* name = luaL_checkstring(L, -1); // colon syntax: arg 1 = node, arg 2 = name

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
