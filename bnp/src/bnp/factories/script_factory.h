#pragma once 

#include <bnp/core/node.hpp>
#include <bnp/components/script.h>
#include <bnp/components/transform.h>

#include <filesystem>

namespace bnp {

	class ScriptFactory {
	public:
		ScriptFactory() = default;

		void load_from_file(Node& node, const std::filesystem::path& path);

	private:
		static void bind_use(Script& script);
		static void bind_node(Script& script);
		static void bind_log(Script& script);
		static void bind_transform(Script& script);
	};

}
