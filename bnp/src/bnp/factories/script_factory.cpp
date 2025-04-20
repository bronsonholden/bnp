#include <bnp/factories/script_factory.h>
#include <bnp/components/physics.h>
#include <bnp/bindings/transform.h>
#include <bnp/bindings/sprite.h>
#include <bnp/bindings/node.h>

#include <fstream>
#include <iostream>
using namespace std;

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace bnp {


	ScriptFactory::ScriptFactory(ResourceManager& _resource_manager)
		: resource_manager(_resource_manager)
	{

	}

	ScriptFactory::~ScriptFactory() {

	}

	void ScriptFactory::load_from_file(Node& node, const std::filesystem::path& path) {
		entt::registry& registry = node.get_registry();
		Scripts& scripts = registry.get_or_emplace<Scripts>(node.get_entity_id(), Scripts{});

		lua_State* L = luaL_newstate();

		luaL_openlibs(L);

		Script script{ L };

		lua_pushlightuserdata(L, (void*)"registry");
		lua_pushlightuserdata(L, &node.get_registry());
		lua_settable(L, LUA_REGISTRYINDEX);

		lua_pushlightuserdata(L, (void*)"entity_id");
		lua_pushinteger(L, static_cast<int>(node.get_entity_id()));
		lua_settable(L, LUA_REGISTRYINDEX);

		lua_pushlightuserdata(L, (void*)"resource_manager");
		lua_pushlightuserdata(L, &resource_manager);
		lua_settable(L, LUA_REGISTRYINDEX);

		bind_metatables(script);
		bind_use(script);
		bind_node(node, script);


		if (luaL_dofile(L, path.string().data()) != LUA_OK) {
			std::cerr << "Lua error: " << lua_tostring(L, -1) << "\n";
			lua_pop(L, 1);
			return;
		}

		node.add_component<Script>(script);

		scripts.list.emplace(path, L);
	}

	void ScriptFactory::bind_use(Script& script) {
		lua_State* L = script.L;

		{
			lua_pushlightuserdata(L, &script);
			lua_pushcclosure(L, [](lua_State* L) -> int {
				Script* script = static_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
				const char* name = luaL_checkstring(L, 1);

				if (strcmp(name, "log") == 0) {
					bind_log(*script);
				}

				return 0;
				}, 1);
		}

		lua_setglobal(L, "use");
	}

	void ScriptFactory::bind_metatables(Script& script) {
		lua_State* L = script.L;

		if (luaL_newmetatable(L, "bnp.Node")) {
			lua_newtable(L);

			lua_pushcfunction(L, l_node_create_child);
			lua_setfield(L, -2, "CreateChild");

			lua_pushcfunction(L, l_node_add_component);
			lua_setfield(L, -2, "AddComponent");

			lua_pushcfunction(L, l_node_get_component);
			lua_setfield(L, -2, "GetComponent");

			lua_setfield(L, -2, "__index");
			lua_pop(L, 1);
		}

		if (luaL_newmetatable(L, "bnp.Sprite")) {
			lua_newtable(L);

			lua_pushcfunction(L, l_sprite_get_slice);
			lua_setfield(L, -2, "GetSlice");

			lua_setfield(L, -2, "__index");
			lua_pop(L, 1);
		}

		if (luaL_newmetatable(L, "bnp.Transform")) {
			lua_newtable(L);

			lua_pushcfunction(L, l_transform_get_position);
			lua_setfield(L, -2, "GetPosition");

			lua_pushcfunction(L, l_transform_set_position);
			lua_setfield(L, -2, "SetPosition");

			lua_setfield(L, -2, "__index");
			lua_pop(L, 1);
		}
	}

	void ScriptFactory::bind_node(Node& node, Script& script) {
		lua_State* L = script.L;

		l_push_script_node(L, node);
		luaL_getmetatable(L, "bnp.Node");
		lua_setmetatable(L, -2);
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

}
