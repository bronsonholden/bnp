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
		void bind_use(Script& script);
		void bind_node(Script& script);
		void bind_log(Script& script);
		void bind_transform(Script& script);
	};

}
