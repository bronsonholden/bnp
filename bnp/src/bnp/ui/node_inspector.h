#pragma once

#include <bnp/core/node.hpp>
#include <imgui.h>
#include <string>

namespace bnp {

class NodeInspector {
public:
	explicit NodeInspector(Node node);
	void render();

private:
	Node node;
};

}
