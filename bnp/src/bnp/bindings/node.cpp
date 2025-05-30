#include <bnp/bindings/node.h>
#include <bnp/components/physics.h>
#include <bnp/components/graphics.h>
#include <bnp/components/script.h>
#include <bnp/components/hierarchy.h>
#include <bnp/components/behavior.h>
#include <bnp/factories/sprite_factory.h>
#include <bnp/managers/resource_manager.h>
#include <bnp/managers/physics_manager.h>

#include <box2d/box2d.h>
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

int l_node_get_entity_id(lua_State* L) {
	// [node]
	Node node = l_pop_script_node(L);
	// []

	lua_pushinteger(L, static_cast<int>(node.get_entity_id()));

	return 1;
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
	// ["Sprite", params]

	lua_getfield(L, -1, "path");

	if (luaL_checkstring(L, -1)) {
		SpriteFactory sprite_factory;
		sprite_factory.load_from_aseprite(node, lua_tostring(L, -1));
		lua_pop(L, 2);
		// []
	}
	else {
		return luaL_error(L, "`path` is required to load sprite");
	}

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

	node.add_component<Transform>(Transform{
		position
		});

	l_push_script_node(L, node);
	luaL_getmetatable(L, "bnp.Transform");
	lua_setmetatable(L, -2);

	return 1;
}

int l_node_add_component_texture(lua_State* L) {
	// [node, "Texture", params]
	Node node = l_pop_script_node(L, 1);
	// ["Texture", params]

	lua_pushlightuserdata(L, (void*)"resource_manager");
	lua_gettable(L, LUA_REGISTRYINDEX);
	// ["Texture", params, udata]
	ResourceManager* resource_manager = static_cast<ResourceManager*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	// ["Texture", params]

	lua_getfield(L, -1, "path");
	std::filesystem::path path = lua_tostring(L, -1);

	Texture texture = resource_manager->load_texture(path.string(), path);
	node.add_component<Texture>(texture);

	lua_pop(L, 2);

	return 0;
}

int l_node_add_component_renderable(lua_State* L) {
	// [node, "Renderable"]
	Node node = l_pop_script_node(L, 1);
	lua_pop(L, 1);
	// []

	node.add_component<Renderable>(true);

	return 0;
}

int l_node_add_component_material(lua_State* L) {
	// [node, "Material", params]
	Node node = l_pop_script_node(L, 1);
	// ["Material", params]

	lua_pushlightuserdata(L, (void*)"resource_manager");
	lua_gettable(L, LUA_REGISTRYINDEX);
	// ["Material", params, udata]
	ResourceManager* resource_manager = static_cast<ResourceManager*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	// ["Material", params]

	if (lua_istable(L, -1)) {
		std::unordered_map<ShaderType, std::filesystem::path> sources;
		std::string resource_id;

		lua_getfield(L, -1, "resourceId");
		if (!luaL_checkstring(L, -1)) {
			return luaL_error(L, "resourceId must be provided");
		}
		else {
			resource_id = lua_tostring(L, -1);
			lua_pop(L, 1);
		}

		lua_getfield(L, -1, "vertex");
		if (luaL_checkstring(L, -1)) sources.emplace(ShaderType::VertexShader, lua_tostring(L, -1));
		lua_pop(L, 1);

		lua_getfield(L, -1, "fragment");
		if (luaL_checkstring(L, -1)) sources.emplace(ShaderType::FragmentShader, lua_tostring(L, -1));
		lua_pop(L, 1);

		node.add_component<Material>(resource_manager->load_material(resource_id, sources));
	}

	return 0;
}

int l_node_add_component_motility(lua_State* L) {
	// [node, "Motility", params]
	Node node = l_pop_script_node(L, 1);
	// ["Motility", params]

	Motility motility{ 1.0f };

	lua_getfield(L, 2, "speed");
	if (lua_isnumber(L, -1)) motility.speed = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, 2, "flying");
	motility.flying = lua_isboolean(L, -1) && lua_toboolean(L, -1);
	lua_pop(L, 1);

	node.add_component<Motility>(motility);

	return 0;
}

int l_node_add_component_parent(lua_State* L) {
	// [node, "Parent", parent]
	Node node = l_pop_script_node(L, 1);
	// ["Parent", params]
	Node parent = l_pop_script_node(L, 2);
	// ["Parent"]
	lua_pop(L, 1);
	// []

	node.add_component<Parent>(Parent{ parent.get_entity_id() });

	l_push_script_node(L, node);

	return 1;
}

int l_node_add_component_water2d(lua_State* L) {
	// [node, "Water2D", params]
	Node node = l_pop_script_node(L, 1);
	// ["Water2D", params]

	lua_getfield(L, 2, "columns");
	lua_getfield(L, 2, "columnWidth");
	lua_getfield(L, 2, "height");
	// ["Water2D", params, columns, columnWidth, height]

	int columns = 0;
	float column_width = 0;
	float height = 0;

	{
		if (!lua_isinteger(L, -3)) return luaL_error(L, "columns must be an integer");
		columns = lua_tointeger(L, -3);
		if (columns < 1) return luaL_error(L, "columns must be greater than zero");
	}

	{
		if (!lua_isnumber(L, -2)) return luaL_error(L, "column width must be a number");
		column_width = lua_tonumber(L, -2);
		if (column_width < 0) return luaL_error(L, "column width must be greater than zero");
	}

	{
		if (!lua_isnumber(L, -1)) return luaL_error(L, "height must be a number");
		height = lua_tonumber(L, -1);
		if (height < 0) return luaL_error(L, "height must be greater than zero");
	}

	lua_pop(L, 5);
	// []

	node.add_component<Water2D>(
		columns,
		column_width,
		std::vector<float>(columns, height),
		std::vector<float>(columns, 0.0f),
		std::vector<float>(columns, height)
	);

	l_push_script_node(L, node);

	return 1;
}

int l_node_add_component_physics_body_2d(lua_State* L) {
	// [node, "PhysicsBody2D", params]
	Node node = l_pop_script_node(L, 1);
	// ["PhysicsBody2D", params]

	lua_pushlightuserdata(L, (void*)"physics_manager");
	lua_gettable(L, LUA_REGISTRYINDEX);
	// ["PhysicsBody2D", params, udata]
	PhysicsManager* physics_manager = static_cast<PhysicsManager*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	// ["PhysicsBody2D", params]

	b2World& world = physics_manager->get_world();
	b2BodyDef body_def;
	b2Vec2 position(0, 0);

	if (node.has_component<Transform>()) {
		auto& transform = node.get_component<Transform>();
		transform.update_world_transform(glm::mat4(1.0f));
		position.x = transform.world_transform[3].x;
		position.y = transform.world_transform[3].y;
	}

	lua_getfield(L, 2, "dynamic");
	// ["PhysicsBody2D", params, dynamic]
	if (lua_isboolean(L, -1) && lua_toboolean(L, -1)) {
		body_def.type = b2_dynamicBody;
	}
	else {
		body_def.type = b2_staticBody;
	}
	// ["PhysicsBody2D", params]

	body_def.awake = true;
	body_def.enabled = true;
	body_def.position = position;

	lua_getfield(L, 2, "gravityScale");
	// ["PhysicsBody2D", params, gravityScale]
	if (lua_isnumber(L, -1)) body_def.gravityScale = lua_tonumber(L, -1);
	lua_pop(L, 1);
	// ["PhysicsBody2D", params]

	lua_pop(L, 2);
	// []

	b2Body* body = world.CreateBody(&body_def);

	node.add_component<PhysicsBody2D>(PhysicsBody2D{ body });

	l_push_script_node(L, node);
	luaL_getmetatable(L, "bnp.PhysicsBody2D");
	lua_setmetatable(L, -2);

	return 1;
}

int l_node_add_component_identity(lua_State* L) {
	// [node, identity]
	Node node = l_pop_script_node(L, 1);
	// [identity]

	std::string species = lua_tostring(L, -1);
	node.add_component<Identity>(species);

	lua_pop(L, 1);
	// []

	return 0;
}

int l_node_add_component_behavior_nest(lua_State* L) {
	// [node, "BehaviorNest", nest]
	Node node = l_pop_script_node(L, 1);
	// ["BehaviorNest", nest]
	Node nest = l_pop_script_node(L, 2);
	// ["BehaviorNest"]
	lua_pop(L, 1);
	// []

	node.add_component<BehaviorNest>(nest.get_entity_id());
	node.get_registry().get_or_emplace<FlowField2D>(nest.get_entity_id(), FlowField2D{
		0.25f,
		{ 60, 60 },
		{ 0, 0 }
		});

	l_push_script_node(L, nest);
	luaL_getmetatable(L, "bnp.Node");
	lua_setmetatable(L, -2);

	return 1;
}

int l_node_add_component_perception(lua_State* L) {
	// [node, "Perception", params]
	Node node = l_pop_script_node(L, 1);
	// ["Perception", params]

	Perception perception;

	lua_pushnil(L);
	// ["Perception", params, nil]

	while (lua_next(L, 2)) {
		// ["Perception, params, key, value]
		const char* key = lua_tostring(L, -2);
		int value = lua_tointeger(L, -1);

		perception.threat.emplace(key, value);

		lua_pop(L, 1); // pop value, keep key for next iteration
		// ["Perception", params, key]
	}

	node.add_component<Perception>(perception);

	// ["Perception", params, key]
	lua_pop(L, 3);
	// []

	return 0;
}

int l_node_add_component_flow_field_2d(lua_State* L) {
	// [node, "FlowField2D", params]
	Node node = l_pop_script_node(L, 1);
	// ["FlowField2D", params]

	// todo: accept params
	node.add_component<FlowField2D>(FlowField2D{
		0.2f,
		{ 40, 40 },
		{ 0, 0 }
		});

	return 0;
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
	else if (strcmp(name, "Texture") == 0) {
		return l_node_add_component_texture(L);
	}
	else if (strcmp(name, "Renderable") == 0) {
		return l_node_add_component_renderable(L);
	}
	else if (strcmp(name, "Material") == 0) {
		return l_node_add_component_material(L);
	}
	else if (strcmp(name, "Motility") == 0) {
		return l_node_add_component_motility(L);
	}
	else if (strcmp(name, "Parent") == 0) {
		return l_node_add_component_parent(L);
	}
	else if (strcmp(name, "Water2D") == 0) {
		return l_node_add_component_water2d(L);
	}
	else if (strcmp(name, "PhysicsBody2D") == 0) {
		return l_node_add_component_physics_body_2d(L);
	}
	else if (strcmp(name, "Identity") == 0) {
		return l_node_add_component_identity(L);
	}
	else if (strcmp(name, "BehaviorNest") == 0) {
		return l_node_add_component_behavior_nest(L);
	}
	else if (strcmp(name, "Perception") == 0) {
		return l_node_add_component_perception(L);
	}
	else if (strcmp(name, "FlowField2D") == 0) {
		return l_node_add_component_flow_field_2d(L);
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
	else if (strcmp(name, "PhysicsBody2D") == 0) {
		if (!node.has_component<PhysicsBody2D>()) {
			lua_pushnil(L);
		}
		else {
			l_push_script_node(L, node);
			luaL_getmetatable(L, "bnp.PhysicsBody2D");
			lua_setmetatable(L, -2);
		}
	}
	else {

		return luaL_error(L, "Unknown/disallowed component: %s", name);
	}
}

}
