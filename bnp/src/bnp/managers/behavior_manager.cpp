#include <bnp/managers/behavior_manager.h>
#include <bnp/components/transform.h>

#include <queue>
#include <glm/glm.hpp>

namespace bnp {

	void BehaviorManager::update(entt::registry& registry, float dt) {
		auto view = registry.view<FlowField2D, Transform>();

		for (auto& entity : view) {
			auto& field = view.get<FlowField2D>(entity);

			if (!field.init) {
				registry.patch<FlowField2D>(entity, [&](FlowField2D& field) {
					generate_cost_field(registry, field);
					generate_direction_field(registry, field);
					});
			}
		}
	}

	glm::vec2 BehaviorManager::smooth_direction(int x, int y, const std::vector<float>& cost_field, glm::ivec2 grid_size) {
		const glm::ivec2 offsets[] = {
			{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
			{ 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
		};

		glm::vec2 result(0.0f);
		float base_cost = cost_field[y * grid_size.x + x];

		for (auto offset : offsets) {
			int nx = x + offset.x;
			int ny = y + offset.y;
			if (nx < 0 || ny < 0 || nx >= grid_size.x || ny >= grid_size.y) continue;

			float neighbor_cost = cost_field[ny * grid_size.x + nx];
			if (neighbor_cost == std::numeric_limits<float>::infinity()) continue;

			float delta = neighbor_cost - base_cost;

			// Weight: small delta = strong influence; large delta = weak influence
			float weight = std::exp(-delta * 2.0f); // Tunable sharpness

			result += glm::normalize(glm::vec2(offset)) * weight;
		}

		return glm::length(result) > 0.001f ? glm::normalize(result) : glm::vec2(0.0f);
	}

	void BehaviorManager::generate_direction_field(entt::registry& registry, FlowField2D& field) {
		const int width = field.grid_size.x;
		const int height = field.grid_size.y;
		field.direction_field.resize(width * height, glm::vec2(0.0f));

		auto index = [&](int x, int y) {
			return y * width + x;
			};

		const glm::ivec2 offsets[] = {
			{ 1, 0}, { -1, 0 }, { 0,  1 }, { 0, -1 }, // 4 cardinal
			{ 1, 1}, { -1, 1 }, { 1, -1 }, {-1, -1 }  // 4 diagonal
		};

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				int i = index(x, y);
				if (field.cost_field[i] == std::numeric_limits<float>::infinity()) {
					field.direction_field[i] = glm::vec2(0.0f);
					continue;
				}
				glm::vec2 dir = smooth_direction(x, y, field.cost_field, field.grid_size);

				field.direction_field[i] = dir;
			}
		}

		field.init = true;
	}

	void BehaviorManager::generate_cost_field(entt::registry& registry, FlowField2D& field) {
		const int width = field.grid_size.x;
		const int height = field.grid_size.y;
		const int total_cells = width * height;

		// Initialize cost field
		field.cost_field.assign(total_cells, std::numeric_limits<float>::infinity());

		std::queue<glm::ivec2> open;
		auto index = [&](int x, int y) { return y * width + x; };

		const int tx = static_cast<int>(std::floor(width / 2));
		const int ty = static_cast<int>(std::floor(height / 2));

		//if (!is_walkable(tx, ty)) return;

		field.cost_field[index(tx, ty)] = 0.0f;
		open.push({ tx, ty });

		const glm::ivec2 offsets[] = {
			{  1,  0 }, // right
			{ -1,  0 }, // left
			{  0,  1 }, // down
			{  0, -1 }, // up
			{  1,  1 }, // down-right
			{ -1,  1 }, // down-left
			{  1, -1 }, // up-right
			{ -1, -1 }  // up-left
		};

		while (!open.empty()) {
			glm::ivec2 current = open.front();
			open.pop();

			float curr_cost = field.cost_field[index(current.x, current.y)];

			for (auto offset : offsets) {
				int nx = current.x + offset.x;
				int ny = current.y + offset.y;

				if (nx < 0 || ny < 0 || nx >= width || ny >= height) continue;
				//if (!is_walkable(nx, ny)) continue;

				float step_cost = (offset.x != 0 && offset.y != 0) ? 1.4142f : 1.0f;
				float new_cost = curr_cost + step_cost;
				int ni = index(nx, ny);

				if (new_cost < field.cost_field[ni]) {
					field.cost_field[ni] = new_cost;
					open.push({ nx, ny });
				}
			}
		}
	}

}
