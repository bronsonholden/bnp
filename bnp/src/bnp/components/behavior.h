#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace bnp {

	struct FlowField2D {
		float cell_size;
		glm::ivec2 grid_size;
		// bottom-left corner of the first cell (bottom-left)
		glm::vec2 origin;
		// world-space position of cell containing target
		glm::vec2 target;


		bool attract = false;
		bool init = false;
		bool dirty = true;

		std::vector<float> cost_field;
		std::vector<glm::vec2> direction_field;
	};

}
