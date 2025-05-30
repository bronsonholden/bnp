#include <bnp/helpers/ui_helper.h>

namespace bnp {

glm::vec4 ui_space_to_world_space(glm::mat4 projection, float screen_width, float screen_height, float x, float y) {
	x = (2.0f * x) / screen_width - 1.0f;
	y = 1.0f - (2.0f * y) / screen_height;
	return glm::inverse(projection) * glm::vec4(x, y, 0.0f, 1.0f);
}

}
