#include <bnp/bindings/transform.h>
#include <bnp/bindings/node.h>
#include <bnp/components/script.h>
#include <bnp/components/transform.h>
#include <bnp/components/physics.h>

#include <glm/glm.hpp>

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
	// [transform, params]
	Node node = l_pop_script_node(L, -2);

	// [transform]
	if (!lua_istable(L, 1)) {
		return luaL_error(L, "SetPosition expects a table with fields x and y");
	}

	lua_getfield(L, 1, "x");
	lua_getfield(L, 1, "y");
	lua_getfield(L, 1, "z");

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

	l_push_script_node(L, node);
	// [node]
	luaL_getmetatable(L, "bnp.Transform");
	// [node, metatable]
	lua_setmetatable(L, -2);
	// [node]

	return 1;
}

int l_transform_get_scale(lua_State* L) {
	// [transform]
	Node node = l_pop_script_node(L, 1);
	// []

	glm::vec3 scale = node.get_component<Transform>().scale;

	lua_newtable(L);
	// [resp]
	lua_pushnumber(L, scale.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, scale.y);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, scale.z);
	lua_setfield(L, -2, "z");
	// [resp]

	return 1;
}

int l_transform_set_scale(lua_State* L) {
	// [transform, params]
	Node node = l_pop_script_node(L, 1);
	// [params]

	glm::vec3 scale = node.get_component<Transform>().scale;

	lua_getfield(L, -1, "x");
	// [params, x]
	scale.x = lua_tonumber(L, -1);
	lua_pop(L, 1);
	// [params]

	lua_getfield(L, -1, "y");
	// [params, y]
	scale.y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	// [params]

	lua_getfield(L, -1, "z");
	// [params, z]
	scale.z = lua_tonumber(L, -1);
	lua_pop(L, 1);
	// [params]

	lua_pop(L, 1);
	// []

	node.get_registry().patch<Transform>(node.get_entity_id(), [&](Transform& t) {
		t.scale = scale;
		t.dirty = true;
		});

	return 0;
}

int l_transform_set_rotation(lua_State* L) {
	// [node, params]
	Node node = l_pop_script_node(L, 1);
	// [params]

	auto& transform = node.get_component<Transform>();

	float radians = 0.0f;

	lua_getfield(L, 1, "radians");
	// [params, radians]
	if (lua_isnumber(L, -1)) radians = lua_tonumber(L, -1);
	lua_pop(L, 1);
	// [params]


	node.get_registry().patch<Transform>(node.get_entity_id(), [=](Transform& t) {
		t.rotation = glm::angleAxis(radians, glm::vec3(0.0f, 0.0f, 1.0f));
		t.dirty = true;
		});

	lua_pop(L, 1);
	// []

	l_push_script_node(L, node);
	// [node]
	luaL_getmetatable(L, "bnp.Transform");
	// [node, metatable]
	lua_setmetatable(L, -2);
	// [node]

	return 1;
}

}
