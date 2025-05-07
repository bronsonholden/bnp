#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace bnp {

	struct World2D {
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
