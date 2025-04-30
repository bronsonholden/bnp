#include <bnp/components/behavior.h>

namespace bnp {

	void FlowField2D::reposition_to_target(glm::vec2 worldspace_target) {
		const int tx = grid_size.x / 2;
		const int ty = grid_size.y / 2;

		float snap_x = std::floor(worldspace_target.x / cell_size) * cell_size;
		float snap_y = std::floor(worldspace_target.y / cell_size) * cell_size;

		if (glm::length(target - glm::vec2(snap_x, snap_y)) > 0.001f) {
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

}
