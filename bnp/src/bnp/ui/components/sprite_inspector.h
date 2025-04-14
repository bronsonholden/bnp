#pragma once

#include <bnp/components/graphics.h>
#include <bnp/core/node.hpp>

#include <imgui.h>

namespace bnp {

	class SpriteInspector {
	public:
		SpriteInspector(Node& node);

		void render();

	private:
		Node& node;
	};

}
