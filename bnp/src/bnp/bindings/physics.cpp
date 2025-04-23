#include <bnp/components/physics.h>
#include <bnp/bindings/physics.h>
#include <bnp/bindings/node.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace bnp {

	int l_physics_body_2d_get_position(lua_State* L) {
		Node node = l_pop_script_node(L);

		const PhysicsBody2D& body = node.get_component<PhysicsBody2D>();
		b2Vec2 position = body.body->GetTransform().p;

		lua_newtable(L);
		lua_pushnumber(L, position.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, position.y);
		lua_setfield(L, -2, "y");

		return 1;
	}

	int l_physics_body_2d_set_position(lua_State* L) {
		// [node, params]
		Node node = l_pop_script_node(L);

		const PhysicsBody2D& body = node.get_component<PhysicsBody2D>();
		b2Vec2 position = body.body->GetTransform().p;

		lua_getfield(L, 2, "x");
		lua_getfield(L, 2, "y");

		if (lua_isnumber(L, -2)) position.x = lua_tonumber(L, -2);
		if (lua_isnumber(L, -1)) position.x = lua_tonumber(L, -1);

		body.body->SetTransform(position, body.body->GetAngle());

		lua_pop(L, 2);
		// []

		return 0;
	}

	int l_physics_body_2d_get_velocity(lua_State* L) {
		// [node]
		Node node = l_pop_script_node(L);
		// []

		const PhysicsBody2D& body = node.get_component<PhysicsBody2D>();

		b2Vec2 velocity = body.body->GetLinearVelocity();

		lua_newtable(L);
		lua_pushnumber(L, velocity.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, velocity.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, body.body->GetAngularVelocity());
		lua_setfield(L, -2, "r");

		return 1;
	}

}
