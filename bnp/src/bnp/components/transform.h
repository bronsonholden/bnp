#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bnp {
	struct Position {
		glm::vec3 value;
	};

	struct Scale {
		glm::vec3 value;
	};

	struct Rotation {
		glm::quat value;
	};
}
