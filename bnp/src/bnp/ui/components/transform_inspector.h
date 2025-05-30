#pragma once

#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>

#include <entt/entt.hpp>
#include <imgui.h>

namespace bnp {

class TransformInspector {
public:
	TransformInspector(Node& node);
	~TransformInspector();

	void render();

private:
	Node& node;
};

}
