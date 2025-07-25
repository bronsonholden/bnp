#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace bnp {

struct Galaxy2D {};

// Properties for rendering a pixel-style 2D planet using a quad
struct Planet2D {
	float rotation;
	float noise_radius;
	float noise_seed;
	glm::vec3 axis;
	glm::vec3 sun_direction;

	float water_depth;
	float coast_depth;
	float mainland_depth;

	// layer colors
	glm::vec3 water_color;
	glm::vec3 coast_color;
	glm::vec3 mainland_color;
	glm::vec3 mountain_color;
	glm::vec3 cloud_color;
	glm::vec3 ice_cap_color;
	glm::vec3 crater_color;
	glm::vec3 crater_rim_color;

	// affects thickness of clouds
	float cloud_depth = -0.125f;
	// base noise sampling radius for cloud formation
	float cloud_radius = 5.0;
	// affects radius of noise sampling near equator. lower exponents
	// have less of a gradient between the equator and poles, while higher
	// exponents exacerbate the difference
	float cloud_radius_equator_exp = 0.4f;
	// affects banding of clouds. lower exponents introduce more banding
	// towards the equator, while greater exponents have more uniform banding
	float cloud_banding_equator_exp = 1.7f;

	// how far to step when sampling for crater axis values
	float crater_step = 1000.0f;
	int num_craters = 0;

	// dot product min/max of spherical coord and axis to show ice cap
	float ice_cap_min = 0.8f;
	float ice_cap_max = 1.0f;
};

// Attach to an entity with a Transform to be tracked with a
// 2D (orthographic projection) camera. The position of the
// camera is kept within a certain pan region, beyond which
// the camera will be repositioned both horizontally and vertically.
struct Camera2DRig {
	// how far from the position the camera may pan. setting either
	// to zero will lock the camera to the attached entity's position
	// along that axis.
	glm::vec2 pan_limit;
	// the worldspace position the camera should be
	glm::vec2 camera_worldspace_position = glm::vec2(0);
	// camera/entity position is multiplied by this per tick to pan
	// to entity position over time
	// todo: alter impl or maybe remove
	glm::vec2 auto_center_impulse = glm::vec2(1.0f);
};

struct World2D {
	float tile_size;

	// world is split up into partitions of this size
	glm::ivec2 partition_size;

	// worldspace coordinates of focus, for loading/activating the objects in
	// the active and neighboring partitions
	glm::vec2 focus;

	// x, y coords of active partition (in partition grid, not worldspace)
	glm::ivec2 get_active_partition() {
		return glm::ivec2(
			std::floor(focus.x / (partition_size.x * tile_size)),
			std::floor(focus.y / (partition_size.y * tile_size))
		);
	}

	bool init = false;
};

struct TilemapIndex {
	std::vector<std::string> resource_ids;
};

struct Tile {
	// resource ID index within corresponding `TilemapIndex`
	uint8_t type;
	// starting top-left, left-to-right, top-to-bottom
	uint8_t slice;
};

struct Tilemap {
	std::vector<int> tiles;
	int width = 0;
	int height = 0;

	// Decomposes solid tile areas into merged rectangles
	std::vector<glm::ivec4> decompose() const {
		std::vector<glm::ivec4> result;
		std::vector<bool> visited(width * height, false);

		auto index = [this](int x, int y) {
			return y * width + x;
			};

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				int i = index(x, y);
				if (tiles[i] == 0 || visited[i]) continue;

				// Expand to the right
				int max_width = 1;
				while (x + max_width < width &&
					tiles[index(x + max_width, y)] != 0 &&
					!visited[index(x + max_width, y)]) {
					++max_width;
				}

				// Expand downward
				int max_height = 1;
				bool expanding = true;
				while (y + max_height < height && expanding) {
					for (int dx = 0; dx < max_width; ++dx) {
						if (tiles[index(x + dx, y + max_height)] == 0 ||
							visited[index(x + dx, y + max_height)]) {
							expanding = false;
							break;
						}
					}
					if (expanding) ++max_height;
				}

				// Mark covered area as visited
				for (int dy = 0; dy < max_height; ++dy) {
					for (int dx = 0; dx < max_width; ++dx) {
						visited[index(x + dx, y + dy)] = true;
					}
				}

				result.emplace_back(x, y, max_width, max_height);
			}
		}

		return result;
	}
};

}
