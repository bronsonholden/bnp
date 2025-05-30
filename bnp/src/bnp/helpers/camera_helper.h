#pragma once

#include <bnp/components/graphics.h>

namespace bnp {

void configure_pixel_perfect_camera(
	Camera& camera,
	int window_width,
	int window_height,
	int pixels_per_unit = 64,
	int target_virtual_width = 480,
	int target_virtual_height = 320
);

}
