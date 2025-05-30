#pragma once

#include <bnp/components/graphics.h>
#include <bnp/core/node.hpp>

#include <imgui.h>

namespace bnp {

class InstancesInspector {
public:
	InstancesInspector(Node& node);

	void render();

private:
	Node& node;

	bool render_instance_transform(Transform& transform);
};

}
