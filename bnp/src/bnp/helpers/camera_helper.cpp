#include <bnp/helpers/camera_helper.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

namespace bnp {

void configure_pixel_perfect_camera(
	Camera& camera,
	int window_width,
	int window_height,
	int pixels_per_unit,
	int target_virtual_width,
	int target_virtual_height
) {
	// 1. Compute best integer scale that fits window
	int scale_x = window_width / target_virtual_width;
	int scale_y = window_height / target_virtual_height;
	int scale = std::max(1, std::min(scale_x, scale_y));

	// 2. Compute the render area in pixels
	int render_width = window_width / scale;
	int render_height = window_height / scale;

	// 3. Convert to world units
	float world_width = static_cast<float>(render_width) / pixels_per_unit;
	float world_height = static_cast<float>(render_height) / pixels_per_unit;

	// 4. Compute centered viewport (for letterboxing/pillarboxing)
	int scaled_render_width = render_width * scale;
	int scaled_render_height = render_height * scale;
	int viewport_x = (window_width - scaled_render_width) / 2;
	int viewport_y = (window_height - scaled_render_height) / 2;

	// 5. Apply viewport
	glViewport(viewport_x, viewport_y, scaled_render_width, scaled_render_height);

	// 6. Apply orthographic projection centered around the camera's position
	camera.perspective = glm::ortho(
		-world_width / 2.0f, world_width / 2.0f,
		-world_height / 2.0f, world_height / 2.0f,
		-1000.0f, 1000.0f
	);

	camera.view = glm::translate(glm::mat4(1.0f), -camera.position);
}
}
