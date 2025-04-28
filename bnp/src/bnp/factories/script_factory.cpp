#include <bnp/factories/script_factory.h>
#include <bnp/components/physics.h>
#include <bnp/bindings/physics.h>
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

	static void copy_table(lua_State* from, lua_State* to, int index_from) {
		// Make sure the index is absolute
		if (index_from < 0) {
			index_from = lua_gettop(from) + index_from + 1;
		}

		lua_newtable(to); // the new table in 'to'

		lua_pushnil(from); // push first key
		while (lua_next(from, index_from) != 0) {
			// key at -2, value at -1 in 'from'

			// --- Copy key ---
			int key_type = lua_type(from, -2);
			switch (key_type) {
			case LUA_TSTRING:
				lua_pushstring(to, lua_tostring(from, -2));
				break;
			case LUA_TNUMBER:
				lua_pushnumber(to, lua_tonumber(from, -2));
				break;
			default:
				lua_pop(from, 1); // pop value, skip unsupported key
				continue;
			}

			// --- Copy value ---
			int val_type = lua_type(from, -1);
			switch (val_type) {
			case LUA_TSTRING:
				lua_pushstring(to, lua_tostring(from, -1));
				break;
			case LUA_TNUMBER:
				if (lua_isinteger(from, -1)) lua_pushinteger(to, lua_tointeger(from, -1));
				else lua_pushnumber(to, lua_tonumber(from, -1));
				break;
			case LUA_TBOOLEAN:
				lua_pushboolean(to, lua_toboolean(from, -1));
				break;
			default:
				lua_pushnil(to); // unsupported value types -> nil
				break;
			}

			// Set key-value into table
			lua_settable(to, -3); // new_table[key] = value

			lua_pop(from, 1); // remove value, keep key for next
		}
	}

	ScriptFactory::ScriptFactory(ResourceManager& _resource_manager, PhysicsManager& _physics_manager)
		: resource_manager(_resource_manager),
		physics_manager(_physics_manager)
	{

	}

	ScriptFactory::~ScriptFactory() {

	}

	lua_State* ScriptFactory::load_from_file(Node& node, const std::filesystem::path& path, lua_State* L) {
		if (L == nullptr) L = luaL_newstate();

		entt::registry& registry = node.get_registry();
		Scripts& scripts = registry.get_or_emplace<Scripts>(node.get_entity_id(), Scripts{});

		if (scripts.list.find(path) != scripts.list.end()) return nullptr;

		lua_pushlightuserdata(L, (void*)"registry");
		lua_pushlightuserdata(L, &node.get_registry());
		lua_settable(L, LUA_REGISTRYINDEX);

		lua_pushlightuserdata(L, (void*)"entity_id");
		lua_pushinteger(L, static_cast<int>(node.get_entity_id()));
		lua_settable(L, LUA_REGISTRYINDEX);

		lua_pushlightuserdata(L, (void*)"resource_manager");
		lua_pushlightuserdata(L, &resource_manager);
		lua_settable(L, LUA_REGISTRYINDEX);

		lua_pushlightuserdata(L, (void*)"physics_manager");
		lua_pushlightuserdata(L, &physics_manager);
		lua_settable(L, LUA_REGISTRYINDEX);

		bind_metatables(L);
		bind_log(L);
		bind_prefab(L);
		bind_node(node, L);

		if (luaL_dofile(L, path.string().data()) != LUA_OK) {
			std::cerr << "Lua error: " << lua_tostring(L, -1) << "\n";
			lua_pop(L, 1);
			return nullptr;
		}

		registry.patch<Scripts>(node.get_entity_id(), [&](Scripts& s) {
			s.list.emplace(path, L);
			});

		return L;
	}

	void ScriptFactory::bind_metatables(lua_State* L) {
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

			lua_pushcfunction(L, l_sprite_set_layer_visible);
			lua_setfield(L, -2, "SetLayerVisible");

			lua_setfield(L, -2, "__index");
			lua_pop(L, 1);
		}

		if (luaL_newmetatable(L, "bnp.Transform")) {
			lua_newtable(L);

			lua_pushcfunction(L, l_transform_get_position);
			lua_setfield(L, -2, "GetPosition");

			lua_pushcfunction(L, l_transform_set_position);
			lua_setfield(L, -2, "SetPosition");

			lua_pushcfunction(L, l_transform_get_scale);
			lua_setfield(L, -2, "GetScale");

			lua_pushcfunction(L, l_transform_set_scale);
			lua_setfield(L, -2, "SetScale");

			lua_setfield(L, -2, "__index");
			lua_pop(L, 1);
		}

		if (luaL_newmetatable(L, "bnp.PhysicsBody2D")) {
			lua_newtable(L);

			lua_pushcfunction(L, l_physics_body_2d_get_position);
			lua_setfield(L, -2, "GetPosition");

			lua_pushcfunction(L, l_physics_body_2d_set_position);
			lua_setfield(L, -2, "SetPosition");

			lua_pushcfunction(L, l_physics_body_2d_get_velocity);
			lua_setfield(L, -2, "GetVelocity");

			lua_pushcfunction(L, l_physics_body_2d_create_box_fixture);
			lua_setfield(L, -2, "CreateBoxFixture");

			lua_setfield(L, -2, "__index");
			lua_pop(L, 1);
		}
	}

	void ScriptFactory::bind_node(Node& node, lua_State* L) {
		l_push_script_node(L, node);
		luaL_getmetatable(L, "bnp.Node");
		lua_setmetatable(L, -2);
		lua_setglobal(L, "node");
	}

	void ScriptFactory::bind_log(lua_State* L) {
		lua_newtable(L);
		lua_pushcfunction(L, [](lua_State* L) -> int {
			const char* msg = luaL_checkstring(L, 1);
			cout << msg << endl;
			return 0;
			});
		lua_setfield(L, -2, "message");
		lua_setglobal(L, "log");
	}

	void ScriptFactory::bind_prefab(lua_State* L) {
		lua_newtable(L);
		lua_pushcfunction(L, [](lua_State* L) -> int {
			// [script, params?]
			if (!lua_istable(L, 2)) lua_newtable(L);
			// [script, params]
			lua_pushlightuserdata(L, (void*)"resource_manager");
			lua_gettable(L, LUA_REGISTRYINDEX);
			// [script, params, udata]
			ResourceManager* resource_manager = static_cast<ResourceManager*>(lua_touserdata(L, -1));
			lua_pop(L, 1);
			lua_pushlightuserdata(L, (void*)"physics_manager");
			lua_gettable(L, LUA_REGISTRYINDEX);
			// [script, params, udata]
			PhysicsManager* physics_manager = static_cast<PhysicsManager*>(lua_touserdata(L, -1));
			lua_pop(L, 1);
			// [script, params]
			lua_pushlightuserdata(L, (void*)"registry");
			lua_gettable(L, LUA_REGISTRYINDEX);
			// [script, params, udata]
			entt::registry& registry = *static_cast<entt::registry*>(lua_touserdata(L, -1));
			lua_pop(L, 1);
			// [script, params]

			ScriptFactory script_factory(*resource_manager, *physics_manager);
			lua_State* L2 = luaL_newstate();
			Node node(registry);

			copy_table(L, L2, -1);
			lua_setglobal(L2, "params");
			// L2: [params]

			std::filesystem::path root = PROJECT_ROOT;
			std::filesystem::path path = root / "bnp" / lua_tostring(L, 1);
			script_factory.load_from_file(node, path, L2);

			lua_pop(L, 2);
			// []
			l_push_script_node(L, node);
			luaL_getmetatable(L, "bnp.Node");
			lua_setmetatable(L, -2);
			// [node]

			return 1;
			});
		lua_setfield(L, -2, "load");
		lua_setglobal(L, "prefab");
	}

}
