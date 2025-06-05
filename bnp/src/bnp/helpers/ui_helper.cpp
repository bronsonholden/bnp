#include <bnp/helpers/ui_helper.h>

namespace bnp {

glm::vec4 ui_space_to_world_space(glm::mat4 projection, float screen_width, float screen_height, float x, float y) {
	x = (2.0f * x) / screen_width - 1.0f;
	y = 1.0f - (2.0f * y) / screen_height;
	return glm::inverse(projection) * glm::vec4(x, y, 0.0f, 1.0f);
}

glm::vec2 world_space_to_ui_space(glm::mat4 projection, float screen_width, float screen_height, float x, float y) {
	glm::vec4 clip_space = projection * glm::vec4(x, y, 0.0f, 1.0f);
	clip_space /= clip_space.w;

	float ui_x = (clip_space.x + 1.0f) * 0.5f * screen_width;
	float ui_y = (1.0f - clip_space.y) * 0.5f * screen_height;  // flip Y for top-left origin

	return glm::vec2(ui_x, ui_y);
}

}
