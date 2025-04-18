#include <bnp/bindings/transform.h>
#include <bnp/bindings/node.h>
#include <bnp/components/script.h>
#include <bnp/components/transform.h>
#include <bnp/components/physics.h>

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

namespace bnp {

	int l_transform_get_position(lua_State* L) {
		Node node = l_pop_script_node(L);

		if (node.has_component<Transform>()) {
			const Transform& transform = node.get_component<Transform>();

			lua_newtable(L);
			lua_pushnumber(L, transform.position.x);
			lua_setfield(L, -2, "x");
			lua_pushnumber(L, transform.position.y);
			lua_setfield(L, -2, "y");
			lua_pushnumber(L, transform.position.z);
			lua_setfield(L, -2, "z");
		}
		else {
			lua_pushnil(L);
		}

		return 1;
	}

	int l_transform_set_position(lua_State* L) {
		Node node = l_pop_script_node(L, -2);

		if (!lua_istable(L, 2)) {
			return luaL_error(L, "SetPosition expects a table with fields x and y");
		}

		lua_getfield(L, 2, "x");
		lua_getfield(L, 2, "y");
		lua_getfield(L, 2, "z");

		if (node.has_component<PhysicsBody2D>()) {
			auto& body = node.get_component<PhysicsBody2D>();

			const b2Transform& t = body.body->GetTransform();

			float x = t.p.x;
			float y = t.p.y;

			if (lua_isnumber(L, -3)) {
				x = static_cast<float>(luaL_checknumber(L, -3));
			}

			if (lua_isnumber(L, -2)) {
				y = static_cast<float>(luaL_checknumber(L, -2));
			}

			body.body->SetTransform(b2Vec2{ x, y }, body.body->GetAngle());
		}
		else if (node.has_component<Transform>()) {
			node.get_registry().patch<Transform>(node.get_entity_id(), [=](Transform& t) {
				if (lua_isnumber(L, -3)) {
					t.position.x = static_cast<float>(luaL_checknumber(L, -3));
				}

				if (lua_isnumber(L, -2)) {
					t.position.y = static_cast<float>(luaL_checknumber(L, -2));
				}

				if (lua_isnumber(L, -1)) {
					t.position.z = static_cast<float>(luaL_checknumber(L, -1));
				}

				t.dirty = true;
				});
		}

		lua_pop(L, 3);

		return 0;
	}

}
