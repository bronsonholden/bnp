#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace bnp {

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
