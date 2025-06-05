#pragma once

#include <glm/glm.hpp>

namespace bnp {

glm::vec4 ui_space_to_world_space(glm::mat4 projection, float screen_width, float screen_height, float x, float y);
glm::vec2 world_space_to_ui_space(glm::mat4 projection, float screen_width, float screen_height, float x, float y);

}
