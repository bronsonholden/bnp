#pragma once 

#include <bnp/core/node.hpp>
#include <bnp/managers/resource_manager.h>
#include <bnp/components/script.h>
#include <bnp/components/transform.h>

#include <filesystem>

namespace bnp {

	class ScriptFactory {
	public:
		ScriptFactory(ResourceManager& resource_manager);
		~ScriptFactory();

		void load_from_file(Node& node, const std::filesystem::path& path);

	private:
		static void bind_metatables(Script& script);
		static void bind_use(Script& script);
		static void bind_node(Script& script);
		static void bind_log(Script& script);

		ResourceManager& resource_manager;
	};

}
