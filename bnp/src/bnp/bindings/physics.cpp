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

int l_physics_body_2d_create_box_fixture(lua_State* L) {
	// [node, params]
	Node node = l_pop_script_node(L, 1);
	// [params]

	const PhysicsBody2D& body = node.get_component<PhysicsBody2D>();

	b2FixtureDef fixture_def;
	b2PolygonShape body_shape;

	lua_getfield(L, 1, "w");
	float hw = lua_tonumber(L, -1) / 2.0f;
	lua_pop(L, 1);
	// [params]

	lua_getfield(L, 1, "h");
	float hh = lua_tonumber(L, -1) / 2.0f;
	lua_pop(L, 1);
	// [params]

	lua_getfield(L, 1, "sensor");
	fixture_def.isSensor = lua_isboolean(L, -1) && lua_toboolean(L, -1);
	lua_pop(L, 1);
	// [params]

	lua_getfield(L, 1, "ignoreContacts");
	if (lua_isboolean(L, -1) && lua_toboolean(L, -1)) fixture_def.filter.maskBits = 0;
	lua_pop(L, 1);

	lua_getfield(L, 1, "restitution");
	if (lua_isnumber(L, -1)) fixture_def.restitution = lua_tonumber(L, -1);
	else fixture_def.restitution = 0.3f;
	lua_pop(L, 1);

	lua_getfield(L, 1, "friction");
	if (lua_isnumber(L, -1)) fixture_def.friction = lua_tonumber(L, -1);
	else fixture_def.friction = 0.8f;
	lua_pop(L, 1);

	lua_getfield(L, 1, "density");
	if (lua_isnumber(L, -1)) fixture_def.density = lua_tonumber(L, -1);
	else fixture_def.density = 5.0f;
	lua_pop(L, 1);

	body_shape.SetAsBox(hw, hh);
	fixture_def.shape = &body_shape;

	body.body->CreateFixture(&fixture_def);

	lua_pop(L, 1);
	// []

	return 0;
}

}
