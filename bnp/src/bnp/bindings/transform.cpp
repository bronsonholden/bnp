#include <bnp/bindings/transform.h>
#include <bnp/components/script.h>
#include <bnp/components/transform.h>
#include <bnp/components/physics.h>

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

namespace bnp {

	int l_transform_get_position(lua_State* L) {
		auto* script = static_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
		auto& transform = script->registry->get<Transform>(script->entity);

		lua_newtable(L);
		lua_pushnumber(L, transform.position.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, transform.position.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, transform.position.z);
		lua_setfield(L, -2, "z");

		return 1;
	}

	int l_transform_set_position(lua_State* L) {
		auto* script = static_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
		auto& transform = script->registry->get<Transform>(script->entity);

		if (!lua_istable(L, 2)) {
			return luaL_error(L, "SetPosition expects a table with fields x and y");
		}

		lua_getfield(L, 2, "x");
		lua_getfield(L, 2, "y");
		lua_getfield(L, 2, "z");

		if (script->registry->all_of<PhysicsBody2D>(script->entity)) {
			auto& body = script->registry->get<PhysicsBody2D>(script->entity);

			const b2Transform& t = body.body->GetTransform();

			float x = t.p.x;
			float y = t.p.y;

			if (lua_isnumber(L, -3)) {
				x = static_cast<float>(luaL_checknumber(L, -3));
			}

			if (lua_isnumber(L, -2)) {
				x = static_cast<float>(luaL_checknumber(L, -2));
			}

			body.body->SetTransform(b2Vec2{ x, y }, body.body->GetAngle());
		}
		else {
			script->registry->patch<Transform>(script->entity, [=](Transform& t) {
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
