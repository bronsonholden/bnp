#pragma once

#include <bnp/core/node.hpp>
#include <entt/entt.hpp>

#include <filesystem>
#include <unordered_map>

extern "C" {
#include <lua.h>
}

namespace bnp {

	struct Scripts {
		std::unordered_map<std::filesystem::path, lua_State*> list;
	};

	struct ScriptNode {
		entt::registry& registry;
		entt::entity entity;
	};

}
