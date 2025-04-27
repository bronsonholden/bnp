#include <bnp/managers/behavior_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/hierarchy.h>

#include <queue>
#include <glm/glm.hpp>

namespace bnp {

	struct CellQueryCallback : public b2QueryCallback {
		bool hit = false;

		bool ReportFixture(b2Fixture* fixture) override {
			hit = hit || fixture->GetBody()->GetType() == b2_staticBody;
			return false;
		}
	};

	BehaviorManager::BehaviorManager(PhysicsManager& _physics_manager)
		: physics_manager(_physics_manager)
	{

	}

	BehaviorManager::~BehaviorManager() {

	}

	void BehaviorManager::update(entt::registry& registry, float dt) {
		update_targets(registry, dt);
		regenerate_if_stale(registry, dt);
	}

	void BehaviorManager::update_targets(entt::registry& registry, float dt) {
		auto view = registry.view<FlowField2D, Transform>();

		for (auto& entity : view) {
			auto& field = view.get<FlowField2D>(entity);
			auto& transform = view.get<Transform>(entity);

			glm::vec4 position = transform.world_transform * glm::vec4(0, 0, 0, 1);

			const int tx = field.grid_size.x / 2;
			const int ty = field.grid_size.y / 2;

			float snap_x = std::floor(position.x / field.cell_size) * field.cell_size;
			float snap_y = std::floor(position.y / field.cell_size) * field.cell_size;

			if (glm::length(field.target - glm::vec2(snap_x, snap_y)) > 0.001f) {
				// reposition the origin
				field.origin.x = snap_x - tx * field.cell_size;
				field.origin.y = snap_y - ty * field.cell_size;

				// reposition the target
				field.target.x = snap_x;
				field.target.y = snap_y;

				field.init = false;
			}

		}
	}


	void BehaviorManager::regenerate_if_stale(entt::registry& registry, float dt) {
		auto view = registry.view<FlowField2D, Transform>();

		for (auto& entity : view) {
			auto& field = view.get<FlowField2D>(entity);
			bool regenerate = !field.init;

			const int tx = field.grid_size.x / 2;
			const int ty = field.grid_size.y / 2;

			// world-space bounding box of the target cell
			glm::vec4 bbox(
				field.target.x,
				field.target.y,
				field.target.x + field.cell_size,
				field.target.y + field.cell_size
			);

			// if target changed grid location
			regenerate = regenerate || field.target.x < bbox.x || field.target.x > bbox.z || field.target.y < bbox.y || field.target.y > bbox.w;

			if (regenerate) {
				registry.patch<FlowField2D>(entity, [&](FlowField2D& field) {
					generate_cost_field(registry, field);
					generate_direction_field(registry, field);
					});
			}
		}
	}

	glm::vec2 BehaviorManager::smooth_direction(int x, int y, const std::vector<float>& cost_field, glm::ivec2 grid_size) {
		constexpr float MAX_DELTA = 5.0f;      // Max allowed cost delta
		constexpr float FALLOFF = 1.1f;         // Exponential falloff sharpness
		constexpr int MIN_GOOD_NEIGHBORS = 8;   // Require at least 3 good neighbors to smooth

		const glm::ivec2 offsets[] = {
		    { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
		    { 1, 1 }, { -1, 1 }, { 1, -1 }, { -1, -1 }
		};

		glm::vec2 smoothed_result(0.0f);
		glm::vec2 best_direction(0.0f);
		float best_neighbor_cost = std::numeric_limits<float>::infinity();
		int good_neighbors = 0;

		float base_cost = cost_field[y * grid_size.x + x];

		for (auto offset : offsets) {
			int nx = x + offset.x;
			int ny = y + offset.y;
			if (nx < 0 || ny < 0 || nx >= grid_size.x || ny >= grid_size.y) continue;

			float neighbor_cost = cost_field[ny * grid_size.x + nx];
			if (neighbor_cost == std::numeric_limits<float>::infinity()) continue;

			float delta = neighbor_cost - base_cost;
			if (delta > MAX_DELTA) continue;

			good_neighbors++;

			float weight = std::exp(-delta * FALLOFF);
			glm::vec2 dir = glm::normalize(glm::vec2(offset));

			if (glm::length(smoothed_result) < glm::length(smoothed_result + dir * weight * 1.1f)) {
				smoothed_result += dir * weight;
			}

			if (neighbor_cost < best_neighbor_cost) {
				best_neighbor_cost = neighbor_cost;
				best_direction = dir;
			}
		}

		if (good_neighbors < MIN_GOOD_NEIGHBORS) {
			// Not enough good neighbors — fallback to best neighbor
			return best_direction;
		}

		return glm::length(smoothed_result) > 0.001f ? glm::normalize(smoothed_result) : glm::vec2(0.0f);
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

				// todo: if not enough neighbors (e.g. a thin hall) don't use any smoothing
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

		field.cost_field.assign(total_cells, std::numeric_limits<float>::infinity());

		std::queue<glm::ivec2> open;
		auto index = [&](int x, int y) { return y * width + x; };

		const int tx = width / 2;
		const int ty = height / 2;

		b2World& world = physics_manager.get_world();

		auto is_blocked = [&](int x, int y) -> bool {
			glm::vec2 cell_origin = field.origin + glm::vec2(x, y) * field.cell_size;
			glm::vec2 cell_max = cell_origin + glm::vec2(field.cell_size);

			b2AABB cell_aabb;
			cell_aabb.lowerBound = b2Vec2(cell_origin.x, cell_origin.y);
			cell_aabb.upperBound = b2Vec2(cell_max.x, cell_max.y);

			constexpr float MAX_ALLOWED_OVERLAP = 0.3f; // Allow up to 30% area overlap
			float cell_area = field.cell_size * field.cell_size;

			struct OverlapQueryCallback : public b2QueryCallback {
				const b2AABB& cell_aabb;
				float overlap_area = 0.0f;

				OverlapQueryCallback(const b2AABB& cell) : cell_aabb(cell) {}

				bool ReportFixture(b2Fixture* fixture) override {
					if (fixture->GetBody()->GetType() != b2_staticBody) {
						return true; // Skip dynamic bodies
					}

					const b2AABB& fixture_aabb = fixture->GetAABB(0); // Assume fixture 0 for simple shapes

					// Calculate intersection AABB
					b2Vec2 lower(
						std::max(cell_aabb.lowerBound.x, fixture_aabb.lowerBound.x),
						std::max(cell_aabb.lowerBound.y, fixture_aabb.lowerBound.y)
					);
					b2Vec2 upper(
						std::min(cell_aabb.upperBound.x, fixture_aabb.upperBound.x),
						std::min(cell_aabb.upperBound.y, fixture_aabb.upperBound.y)
					);

					if (lower.x < upper.x && lower.y < upper.y) {
						// Valid intersection
						float width = upper.x - lower.x;
						float height = upper.y - lower.y;
						overlap_area += width * height;
					}

					return true; // Continue searching other fixtures
				}
			};

			OverlapQueryCallback callback(cell_aabb);
			world.QueryAABB(&callback, cell_aabb);

			float overlap_ratio = callback.overlap_area / cell_area;

			return overlap_ratio > MAX_ALLOWED_OVERLAP;
			};

		// ==== INITIAL FLOOD-FILL SETUP ====

		// Check if target cell is walkable
		if (!is_blocked(tx, ty)) {
			field.cost_field[index(tx, ty)] = 0.0f;
			open.push({ tx, ty });
		}
		else {
			// Try to find the nearest non-blocked neighbor
			const glm::ivec2 offsets[] = {
			    {  1,  0 }, { -1,  0 }, {  0,  1 }, {  0, -1 },
			    {  1,  1 }, { -1,  1 }, {  1, -1 }, { -1, -1 }
			};

			bool found = false;
			for (int i = 0; i < 5 && !found; ++i) {
				for (auto offset : offsets) {
					int nx = tx + i * offset.x;
					int ny = ty + i * offset.y;
					if (nx < 0 || ny < 0 || nx >= width || ny >= height) continue;

					if (!is_blocked(nx, ny)) {
						field.cost_field[index(nx, ny)] = 0.0f;
						open.push({ nx, ny });
						found = true;
						break;
					}
				}
			}

			if (!found) {
				// No available start, field cannot be generated
				return;
			}
		}

		// ==== FLOOD-FILL ====

		const glm::ivec2 offsets[] = {
		    {  1,  0 }, { -1,  0 }, {  0,  1 }, {  0, -1 },
		    {  1,  1 }, { -1,  1 }, {  1, -1 }, { -1, -1 }
		};

		while (!open.empty()) {
			glm::ivec2 current = open.front();
			open.pop();

			float curr_cost = field.cost_field[index(current.x, current.y)];

			for (auto offset : offsets) {
				int nx = current.x + offset.x;
				int ny = current.y + offset.y;
				if (nx < 0 || ny < 0 || nx >= width || ny >= height) continue;

				if (is_blocked(nx, ny)) continue;

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
