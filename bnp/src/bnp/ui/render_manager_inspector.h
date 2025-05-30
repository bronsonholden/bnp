#pragma once

#include <bnp/managers/render_manager.h>

namespace bnp {

class RenderManagerInspector {
public:
	RenderManagerInspector(RenderManager& render_manager);
	void render();

private:
	RenderManager& render_manager;
};

}
