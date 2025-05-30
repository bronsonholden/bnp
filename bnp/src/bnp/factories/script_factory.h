#pragma once 

#include <bnp/core/node.hpp>
#include <bnp/managers/resource_manager.h>
#include <bnp/managers/physics_manager.h>
#include <bnp/components/script.h>
#include <bnp/components/transform.h>

#include <filesystem>

namespace bnp {

class ScriptFactory {
public:
	ScriptFactory(ResourceManager& resource_manager, PhysicsManager& physics_manager);
	~ScriptFactory();

	lua_State* load_from_file(Node& node, const std::filesystem::path& path, lua_State* L = nullptr);

private:
	static void bind_metatables(lua_State* L);
	static void bind_use(lua_State* L);
	static void bind_prefab(lua_State* L);
	static void bind_node(Node& node, lua_State* L);
	static void bind_log(lua_State* L);

	ResourceManager& resource_manager;
	PhysicsManager& physics_manager;
};

}
