#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace bnp {

	struct FlowField2D {
		float cell_size;
		glm::ivec2 grid_size;
		// top-left corner of the first cell
		glm::vec2 origin;

		bool init = false;
		bool dirty = true;

		std::vector<float> cost_field;
		std::vector<glm::vec2> direction_field;
	};

}
