#include <bnp/factories/script_factory.h>
#include <bnp/components/physics.h>
#include <bnp/bindings/transform.h>

#include <fstream>
#include <iostream>
using namespace std;

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace bnp {

	void ScriptFactory::load_from_file(Node& node, const std::filesystem::path& path) {
		lua_State* L = luaL_newstate();

		luaL_openlibs(L);

		Script script{
			L,
			&node.get_registry(),
			node.get_entity_id()
		};

		bind_use(script);
		bind_node(script);

		if (node.has_component<Transform>()) {
			bind_transform(script);
		}

		if (luaL_dofile(L, path.string().data()) != LUA_OK) {
			std::cerr << "Lua error: " << lua_tostring(L, -1) << "\n";
			lua_pop(L, 1);
			return;
		}

		node.add_component<Script>(script);
	}

	void ScriptFactory::bind_use(Script& script) {
		lua_State* L = script.L;

		lua_pushlightuserdata(L, &script);

		lua_pushcclosure(L, [](lua_State* L) -> int {
			Script* script = static_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
			const char* name = luaL_checkstring(L, 1);

			if (strcmp(name, "log") == 0) {
				bind_log(*script);
			}

			return 0;
			}, 1);

		lua_setglobal(L, "use");
	}

	void ScriptFactory::bind_node(Script& script) {
		lua_State* L = script.L;
		entt::registry& registry = *script.registry;
		entt::entity entity = script.entity;

		lua_newtable(L); // Stack: [node]

		lua_pushlightuserdata(L, &script);

		lua_pushcclosure(L, [](lua_State* L) -> int {
			// Upvalue access
			auto* script = static_cast<Script*>(lua_touserdata(L, lua_upvalueindex(2)));

			if (!lua_isstring(L, 2)) {
				return luaL_error(L, "Expected component name string as second argument");
			}

			std::string name = lua_tostring(L, 2);

			if (name == "Sprite") {
				std::cout << "Adding Sprite to entity\n";
			}
			else if (name == "Transform") {
				std::cout << "Adding Transform to entity\n";
			}
			else if (name == "PhysicsBody2D") {
				std::cout << "Adding PhysicsBody2D to entity\n";
			}
			else {
				return luaL_error(L, "Unknown/disallowed component: %s", name.c_str());
			}

			return 0;
			}, 1);

		lua_setfield(L, -2, "addComponent");

		lua_setglobal(L, "node");
	}

	void ScriptFactory::bind_log(Script& script) {
		lua_State* L = script.L;
		lua_newtable(L);
		lua_pushcfunction(L, [](lua_State* L) -> int {
			const char* msg = luaL_checkstring(L, 1);
			cout << msg << endl;
			return 0;
			});
		lua_setfield(L, -2, "message");
		lua_setglobal(L, "log");
	}

	void ScriptFactory::bind_transform(Script& script) {
		lua_State* L = script.L;
		entt::registry& registry = *script.registry;
		entt::entity entity = script.entity;

		if (!registry.all_of<Transform>(entity)) return;

		// Get `node` global
		lua_getglobal(L, "node"); // stack: node

		if (!lua_istable(L, -1)) {
			lua_pop(L, 1);
			std::cerr << "`node` is not a table!\n";
			return;
		}

		lua_newtable(L); // transform table

		// GetPosition
		lua_pushlightuserdata(L, &script);
		lua_pushcclosure(L, l_transform_get_position, 1);
		lua_setfield(L, -2, "GetPosition");

		// SetPosition (expects table with x and y)
		lua_pushlightuserdata(L, &script);
		lua_pushcclosure(L, l_transform_set_position, 1);
		lua_setfield(L, -2, "SetPosition");

		// Set `transform` into `node`
		lua_setfield(L, -2, "transform"); // node.transform = transform table

		lua_pop(L, 1);
	}

}
