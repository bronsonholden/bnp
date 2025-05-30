#pragma once

#include <bnp/components/behavior.h>
#include <bnp/core/node.hpp>

#include <imgui.h>

namespace bnp {

class BehaviorBrainInspector {
public:
	BehaviorBrainInspector(Node& node);

	void render();

private:
	Node& node;
};

}
