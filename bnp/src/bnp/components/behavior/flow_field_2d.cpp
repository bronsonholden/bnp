#include <bnp/components/behavior.h>
#include <bnp/components/global.h>
#include <bnp/components/physics.h>

#include <queue>
#include <random>
#include <cmath>

#define INDEX(x, y) ((y) * width + (x))

namespace bnp {

glm::vec4 FlowField2D::get_aabb() {
	glm::vec2 top_right = origin + glm::vec2(grid_size.x * cell_size, grid_size.y * cell_size);
	return glm::vec4(origin.x, origin.y, top_right.x, top_right.y);
}

glm::vec2 FlowField2D::sample_direction(glm::vec2 worldspace_position) {
	if (!init) {
		return glm::vec2(0.0f);
	}

	glm::vec2 field_position = glm::vec2(worldspace_position) - origin;

	int fx = std::floor(field_position.x / cell_size);
	int fy = std::floor(field_position.y / cell_size);

	if (fx < 0 || fx >= grid_size.x || fy < 0 || fy >= grid_size.y) {
		return glm::vec2(0.0f);
	}

	int idx = fy * grid_size.x + fx;
	glm::vec2 dir = direction_field.at(idx);
	float cost = cost_field.at(idx);
	bool blocked_left = false, blocked_right = false, blocked_up = false, blocked_down = false;

	int tx = grid_size.x / 2;
	int ty = grid_size.y / 2;

	if (cost == std::numeric_limits<float>::infinity() || glm::length(dir) < 0.001f) {
		const glm::ivec2 offsets[] = {
			{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
			{ 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
		};

		glm::vec2 best_dir(0);
		float best_cost = -std::numeric_limits<float>::infinity();

		for (auto& offset : offsets) {
			int ox = fx + offset.x;
			int oy = fy + offset.y;

			if (ox < 0 || ox >= grid_size.x || oy < 0 || oy >= grid_size.y) {
				continue;
			}

			int idx = oy * grid_size.x + ox;
			glm::vec2 neighbor_dir = direction_field.at(idx);
			float cost = cost_field.at(idx);

			if (cost == std::numeric_limits<float>::infinity() || glm::length(neighbor_dir) < 0.001f) {
				continue;
			}

			if (cost > best_cost) {
				best_dir = glm::normalize(glm::vec2(offset.x, offset.y));
				best_cost = cost;
			}
		}

		dir = best_dir;
	}

	dir = adjust_for_blocked_neighbors(fx, fy, field_position, dir);

	glm::ivec2 invalid = glm::isnan(dir);

	if (!invalid.x && !invalid.y) return dir;

	return glm::vec2(0.0f);
}

glm::vec2 FlowField2D::adjust_for_blocked_neighbors(
	int fx, int fy,
	glm::vec2 field_position,
	glm::vec2 dir
) {
	const int width = grid_size.x;
	const int height = grid_size.y;

	bool blocked_left = false, blocked_right = false;
	bool blocked_up = false, blocked_down = false;
	bool blocked_ul = false, blocked_ur = false;
	bool blocked_dl = false, blocked_dr = false;

	const glm::ivec2 offsets[] = {
		{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
		{ -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 }
	};

	for (auto& offset : offsets) {
		int ox = fx + offset.x;
		int oy = fy + offset.y;

		if (ox < 0 || ox >= width || oy < 0 || oy >= height) continue;

		int idx = oy * width + ox;
		float cost = cost_field[idx];

		if (cost == std::numeric_limits<float>::infinity()) {
			if (offset == glm::ivec2{ -1, 0 }) blocked_left = true;
			if (offset == glm::ivec2{ 1, 0 }) blocked_right = true;
			if (offset == glm::ivec2{ 0, -1 }) blocked_down = true;
			if (offset == glm::ivec2{ 0, 1 }) blocked_up = true;
			if (offset == glm::ivec2{ -1, -1 }) blocked_dl = true;
			if (offset == glm::ivec2{ 1, -1 }) blocked_dr = true;
			if (offset == glm::ivec2{ -1, 1 }) blocked_ul = true;
			if (offset == glm::ivec2{ 1, 1 }) blocked_ur = true;
		}
	}

	glm::vec2 cell_position = field_position - glm::vec2(fx * cell_size, fy * cell_size);
	float px = cell_position.x / cell_size;
	float py = cell_position.y / cell_size;

	// Cardinals
	if (blocked_left && dir.x < 0 && px < 0.2f) dir.x = 0.1f;
	if (blocked_right && dir.x > 0 && px > 0.8f) dir.x = -0.1f;
	if (blocked_down && dir.y < 0 && py < 0.2f) dir.y = 0.1f;
	if (blocked_up && dir.y > 0 && py > 0.8f) dir.y = -0.1f;

	// Diagonals
	if (blocked_ul && dir.x < 0 && dir.y > 0 && px < 0.3f && py > 0.7f) {
		dir.x = 0.1f;
		dir.y = -0.1f;
	}
	if (blocked_ur && dir.x > 0 && dir.y > 0 && px > 0.7f && py > 0.7f) {
		dir.x = -0.1f;
		dir.y = -0.1f;
	}
	if (blocked_dl && dir.x < 0 && dir.y < 0 && px < 0.3f && py < 0.3f) {
		dir.x = 0.1f;
		dir.y = 0.1f;
	}
	if (blocked_dr && dir.x > 0 && dir.y < 0 && px > 0.7f && py < 0.3f) {
		dir.x = -0.1f;
		dir.y = 0.1f;
	}

	return glm::normalize(dir);
}

glm::vec2 FlowField2D::sample_reverse_direction(glm::vec2 worldspace_position) {
	if (!init) return glm::vec2(0);

	glm::vec2 field_position = worldspace_position - origin;

	int fx = std::floor(field_position.x / cell_size);
	int fy = std::floor(field_position.y / cell_size);

	if (fx < 0 || fx >= grid_size.x || fy < 0 || fy >= grid_size.y) {
		return glm::vec2(0.0f);
	}

	int idx = fy * grid_size.x + fx;
	glm::vec2 dir = reverse_field.at(idx);
	float cost = cost_field.at(idx);
	bool blocked_left = false, blocked_right = false, blocked_up = false, blocked_down = false;

	if (cost == std::numeric_limits<float>::infinity() || glm::length(dir) < 0.001f) {
		const glm::ivec2 offsets[] = {
			{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
			{ 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
		};

		glm::vec2 best_dir(0);
		float best_cost = -std::numeric_limits<float>::infinity();

		for (auto& offset : offsets) {
			int ox = fx + offset.x;
			int oy = fy + offset.y;

			if (ox < 0 || ox >= grid_size.x || oy < 0 || oy >= grid_size.y) {
				continue;
			}

			int idx = oy * grid_size.x + ox;
			glm::vec2 neighbor_dir = reverse_field.at(idx);
			float cost = cost_field.at(idx);

			if (cost == std::numeric_limits<float>::infinity() || glm::length(neighbor_dir) < 0.001f) {
				continue;
			}

			if (cost > best_cost) {
				best_dir = glm::normalize(glm::vec2(offset.x, offset.y));
				best_cost = cost;
			}
		}

		dir = best_dir;
	}

	return adjust_for_blocked_neighbors(fx, fy, field_position, dir);
}

glm::vec2 FlowField2D::sample_wander_direction(glm::vec2 worldspace_position) {
	if (!init) return glm::vec2(0);

	glm::vec2 field_position = worldspace_position - origin;

	int fx = std::floor(field_position.x / cell_size);
	int fy = std::floor(field_position.y / cell_size);

	if (fx < 0 || fx >= grid_size.x || fy < 0 || fy >= grid_size.y) {
		return glm::vec2(0.0f);
	}

	int idx = fy * grid_size.x + fx;
	glm::vec2 dir = wander_field.at(idx);
	float cost = cost_field.at(idx);
	bool blocked_left = false, blocked_right = false, blocked_up = false, blocked_down = false;

	if (cost == std::numeric_limits<float>::infinity() || glm::length(dir) < 0.001f) {
		const glm::ivec2 offsets[] = {
			{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
			{ 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
		};

		glm::vec2 best_dir(0);
		float best_cost = -std::numeric_limits<float>::infinity();

		for (auto& offset : offsets) {
			int ox = fx + offset.x;
			int oy = fy + offset.y;

			if (ox < 0 || ox >= grid_size.x || oy < 0 || oy >= grid_size.y) {
				continue;
			}

			int idx = oy * grid_size.x + ox;
			glm::vec2 neighbor_dir = wander_field.at(idx);
			float cost = cost_field.at(idx);

			if (cost == std::numeric_limits<float>::infinity() || glm::length(neighbor_dir) < 0.001f) {
				continue;
			}

			if (cost > best_cost) {
				best_dir = glm::normalize(glm::vec2(offset.x, offset.y));
				best_cost = cost;
			}
		}

		dir = best_dir;
	}

	return adjust_for_blocked_neighbors(fx, fy, field_position, dir);
}

void FlowField2D::reposition_to_target(glm::vec2 worldspace_target) {
	const int tx = grid_size.x / 2;
	const int ty = grid_size.y / 2;

	glm::vec2 current_target(origin.x + tx * cell_size, origin.y + ty * cell_size);

	float snap_x = std::floor(worldspace_target.x / cell_size) * cell_size;
	float snap_y = std::floor(worldspace_target.y / cell_size) * cell_size;

	if (glm::length(current_target - glm::vec2(snap_x, snap_y)) > 0.001f) {
		// reposition the origin
		origin.x = snap_x - tx * cell_size;
		origin.y = snap_y - ty * cell_size;

		// reposition the target
		target.x = snap_x;
		target.y = snap_y;

		init = false;
	}
}

// get the cell x, y for a flow field's configuration. if the given position is
// out of the flow field bounds, false is returned and `result` is unchanged.
bool FlowField2D::get_cell_position(glm::ivec2& result, glm::vec2 worlspace_position) {
	int x = std::floor(worlspace_position.x / cell_size) * cell_size;
	int y = std::floor(worlspace_position.y / cell_size) * cell_size;

	if (x < 0 || x >= grid_size.x || y < 0 || y > grid_size.y) return false;

	result.x = x;
	result.y = y;
}

bool FlowField2D::is_target_cell(glm::vec2 worldspace_position) {
	glm::ivec2 cell;
	bool valid = get_cell_position(cell, worldspace_position);

	if (!valid) return false;

	int tx = grid_size.x / 2;
	int ty = grid_size.x / 2;

	return tx == cell.x && ty == cell.y;
}

void FlowField2D::generate_cost_field(entt::registry& registry) {
	auto& global = registry.get<Global>(registry.view<Global>().front());
	b2World& world = *global.world;

	const int width = grid_size.x;
	const int height = grid_size.y;
	const int total_cells = width * height;

	cost_field.assign(total_cells, std::numeric_limits<float>::infinity());

	std::vector<bool> blocked(total_cells, false);
	std::vector<bool> underwater(total_cells, false);

	// ===== Precompute blocked and underwater states =====
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int idx = y * width + x;
			glm::vec2 cell_origin = origin + glm::vec2(x, y) * cell_size;
			glm::vec2 cell_max = cell_origin + glm::vec2(cell_size);

			// Check physics collisions
			{
				b2AABB cell_aabb;
				cell_aabb.lowerBound = b2Vec2(cell_origin.x, cell_origin.y);
				cell_aabb.upperBound = b2Vec2(cell_max.x, cell_max.y);

				constexpr float MAX_ALLOWED_OVERLAP = 0.3f;
				float cell_area = cell_size * cell_size;

				struct OverlapQueryCallback : public b2QueryCallback {
					const b2AABB& cell_aabb;
					float overlap_area = 0.0f;

					OverlapQueryCallback(const b2AABB& cell) : cell_aabb(cell) {}

					bool ReportFixture(b2Fixture* fixture) override {
						if (fixture->GetBody()->GetType() != b2_staticBody || fixture->IsSensor())
							return true;

						const b2AABB& fixture_aabb = fixture->GetAABB(0);
						b2Vec2 lower(
							std::max(cell_aabb.lowerBound.x, fixture_aabb.lowerBound.x),
							std::max(cell_aabb.lowerBound.y, fixture_aabb.lowerBound.y));
						b2Vec2 upper(
							std::min(cell_aabb.upperBound.x, fixture_aabb.upperBound.x),
							std::min(cell_aabb.upperBound.y, fixture_aabb.upperBound.y));

						if (lower.x < upper.x && lower.y < upper.y) {
							overlap_area += (upper.x - lower.x) * (upper.y - lower.y);
						}

						return true;
					}
				};

				OverlapQueryCallback callback(cell_aabb);
				world.QueryAABB(&callback, cell_aabb);

				blocked[idx] = (callback.overlap_area / cell_area) > MAX_ALLOWED_OVERLAP;
			}

			// Check underwater
			{
				auto view = registry.view<Transform, Water2D>();
				for (auto entity : view) {
					auto& water = view.get<Water2D>(entity);
					auto& transform = view.get<Transform>(entity);
					float water_w = water.columns * water.column_width;
					float water_left = transform.world_transform[3].x - water_w / 2.0f;
					float water_right = transform.world_transform[3].x + water_w / 2.0f;

					if (cell_max.x < water_left || cell_origin.x > water_right) continue;
					if (cell_max.y < transform.world_transform[3].y) continue;

					int min_col = std::floor((cell_origin.x - water_left) / water.column_width);
					int max_col = std::floor((cell_max.x - water_left) / water.column_width);
					int columns_in_cell = max_col - min_col + 1;

					float total_water_height = 0.0f;
					for (int i = min_col; i <= max_col; ++i) {
						if (i < 0 || i >= water.columns) continue;
						total_water_height += water.height[i] - cell_origin.y;
					}

					float overlap = total_water_height / (columns_in_cell * cell_size);
					if (overlap > 0.3f) {
						underwater[idx] = true;
						break;
					}
				}
			}
		}
	}

	// ==== Initial flood-fill setup ====
	std::queue<glm::ivec2> open;

	const int tx = width / 2;
	const int ty = height / 2;

	auto is_invalid = [&](int x, int y) {
		int i = INDEX(x, y);
		return blocked[i] || underwater[i];
		};

	if (!is_invalid(tx, ty)) {
		cost_field[INDEX(tx, ty)] = 0.0f;
		open.push({ tx, ty });
	}
	else {
		const glm::ivec2 offsets[] = {
			{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
			{ 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
		};
		bool found = false;
		for (int i = 0; i < 5 && !found; ++i) {
			for (auto& offset : offsets) {
				int nx = tx + i * offset.x;
				int ny = ty + i * offset.y;
				if (nx < 0 || ny < 0 || nx >= width || ny >= height) continue;
				if (!is_invalid(nx, ny)) {
					cost_field[INDEX(nx, ny)] = 0.0f;
					open.push({ nx, ny });
					found = true;
					break;
				}
			}
		}
		if (!found) return;
	}

	// ==== Flood fill ====
	const glm::ivec2 offsets[] = {
		{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
		{ 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
	};

	while (!open.empty()) {
		glm::ivec2 current = open.front();
		open.pop();

		float curr_cost = cost_field[INDEX(current.x, current.y)];

		for (auto offset : offsets) {
			int nx = current.x + offset.x;
			int ny = current.y + offset.y;
			if (nx < 0 || ny < 0 || nx >= width || ny >= height) continue;
			if (is_invalid(nx, ny)) continue;

			float step_cost = (offset.x != 0 && offset.y != 0) ? 1.4142f : 1.0f;
			float new_cost = curr_cost + step_cost;
			int ni = INDEX(nx, ny);

			if (new_cost < cost_field[ni]) {
				cost_field[ni] = new_cost;
				open.push({ nx, ny });
			}
		}
	}
}


glm::vec2 FlowField2D::smooth_direction(
	int x, int y,
	const std::vector<float>& cost_field,
	glm::ivec2 grid_size,
	bool attract
) {
	constexpr float MAX_DELTA = 8.0f;    // Max allowed delta difference
	constexpr float FALLOFF = 1.1f;       // Exponential falloff sharpness
	int MIN_GOOD_NEIGHBORS = attract ? 8 : 3; // Require at least this many for smoothing

	const glm::ivec2 offsets[] = {
	    { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
	    { 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
	};

	glm::vec2 smoothed_result(0.0f);
	glm::vec2 best_direction(0.0f);
	float best_neighbor_cost = attract
		? std::numeric_limits<float>::infinity()
		: -std::numeric_limits<float>::infinity();

	int good_neighbors = 0;

	float base_cost = cost_field[y * grid_size.x + x];
	if (base_cost == std::numeric_limits<float>::infinity()) {
		return glm::vec2(0.0f); // Blocked cell, no direction
	}

	// Threat assumed at center
	glm::vec2 threat_pos(grid_size.x * 0.5f, grid_size.y * 0.5f);
	glm::vec2 from_threat = glm::normalize(glm::vec2(x, y) - threat_pos);

	for (auto offset : offsets) {
		int nx = x + offset.x;
		int ny = y + offset.y;
		if (nx < 0 || ny < 0 || nx >= grid_size.x || ny >= grid_size.y) continue;

		float neighbor_cost = cost_field[ny * grid_size.x + nx];

		// Detect walls in cardinal directions
		if (neighbor_cost == std::numeric_limits<float>::infinity()) {
			continue; // No smoothing contribution from blocked neighbor
		}

		float delta = neighbor_cost - base_cost;

		bool valid = attract
			? (delta < MAX_DELTA)
			: (delta > 0.0f);

		if (!valid) continue;

		good_neighbors++;

		float cost_weight = attract
			? std::exp(-delta * FALLOFF)
			: std::exp(delta * FALLOFF);

		glm::vec2 neighbor_dir = glm::normalize(glm::vec2(offset));

		float escape_alignment = glm::dot(neighbor_dir, from_threat);
		escape_alignment = glm::clamp(escape_alignment, 0.0f, 1.0f);

		float final_weight = attract
			? cost_weight
			: cost_weight * escape_alignment;

		smoothed_result += neighbor_dir * final_weight;

		// Pick best single neighbor
		bool better = attract
			? (neighbor_cost < best_neighbor_cost)
			: (neighbor_cost > best_neighbor_cost);

		if (better) {
			best_neighbor_cost = neighbor_cost;
			best_direction = neighbor_dir;
		}
	}

	if (good_neighbors < MIN_GOOD_NEIGHBORS) {
		return best_direction;
	}

	return glm::length(smoothed_result) > 0.001f ? glm::normalize(smoothed_result) : best_direction;
}

void FlowField2D::generate_direction_field(entt::registry& registry) {
	const int width = grid_size.x;
	const int height = grid_size.y;

	direction_field.resize(width * height, glm::vec2(0.0f));
	reverse_field.resize(width * height, glm::vec2(0.0f));

	const glm::ivec2 offsets[] = {
		{ 1, 0}, { -1, 0 }, { 0,  1 }, { 0, -1 }, // 4 cardinal
		{ 1, 1}, { -1, 1 }, { 1, -1 }, {-1, -1 }  // 4 diagonal
	};

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int i = INDEX(x, y);
			if (cost_field[i] == std::numeric_limits<float>::infinity()) {
				direction_field[i] = glm::vec2(0.0f);
				reverse_field[i] = glm::vec2(0.0f);
				continue;
			}

			direction_field[i] = smooth_direction(x, y, cost_field, grid_size, true);
			reverse_field[i] = smooth_direction(x, y, cost_field, grid_size, false);
		}
	}

	generate_wander_field(0);

	init = true;
}

void FlowField2D::generate_wander_field(float frequency, float speed, float swirl_scale) {
	const int width = grid_size.x;
	const int height = grid_size.y;

	wander_field.resize(width * height, glm::vec2(0.0f));

	std::mt19937 rng(33); // Seeded RNG
	std::uniform_real_distribution<float> offset(0.0f, 100.0f); // Large range offsets to desync waves

	float ox1 = offset(rng);
	float oy1 = offset(rng);
	float ox2 = offset(rng);
	float oy2 = offset(rng);

	auto noise = [&](float x, float y, float t) -> float {
		return std::sin((x + ox1) * frequency + t * speed)
			+ std::cos((y + oy1) * frequency - t * speed)
			+ std::sin((x + ox2 + y + oy2) * frequency * 0.5f + t * speed * 0.7f);
		};

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (cost_field[y * width + x] == std::numeric_limits<float>::infinity()) {
				wander_field[y * width + x] = glm::vec2(0);
				continue;
			}

			float px = static_cast<float>(x) + (wander_time / 0.6f);
			float py = static_cast<float>(y) + (std::cos(wander_time * 2.0f));

			float dx = noise(px + 3.1f, py, (wander_time + std::cos(0.1f * y)));
			float dy = noise(px, py + 2.3f, (wander_time + std::sin(0.1f * x)));

			glm::vec2 dir(dx, dy);
			wander_field[y * width + x] = glm::normalize(dir) * swirl_scale;
		}
	}
}



}
