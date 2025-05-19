#include <bnp/managers/water_2d_manager.h>

namespace bnp {

	void Water2DManager::update(entt::registry& registry, float dt) {
		auto view = registry.view<Water2D>();

		for (int i = 0; i < 10; ++i) {
			for (auto entity : view) {
				auto& water = view.get<Water2D>(entity);
				auto& pos = water.height;
				auto& vel = water.velocity;
				auto& rest = water.base_height;
				int cols = water.columns;

				// Apply spring forces
				for (int i = 0; i < cols; ++i) {
					float displacement = pos[i] - rest[i];
					float accel = -tension * displacement - vel[i] * damping;
					vel[i] += accel * dt;
					pos[i] += vel[i] * dt * 10;
				}

				// Wave propagation
				std::vector<float> leftDelta(cols, 0.f), rightDelta(cols, 0.f);

				for (int pass = 0; pass < smoothing_passes; ++pass) {
					for (int i = 0; i < cols; ++i) {
						if (i > 0) {
							leftDelta[i] = spread * (pos[i] - pos[i - 1]);
							vel[i - 1] += leftDelta[i] * dt;
						}
						if (i < cols - 1) {
							rightDelta[i] = spread * (pos[i] - pos[i + 1]);
							vel[i + 1] += rightDelta[i] * dt;
						}
					}
					for (int i = 0; i < cols; ++i) {
						if (i > 0) pos[i - 1] += leftDelta[i] * dt;
						if (i < cols - 1) pos[i + 1] += rightDelta[i] * dt;
					}
				}
			}
		}

	}


	void Water2DManager::disturb(entt::registry& registry, entt::entity entity, int columnIndex, float velocity) {
		auto& water = registry.get<Water2D>(entity);
		if (columnIndex >= 0 && columnIndex < water.columns) {
			water.velocity[columnIndex] += velocity;
		}
	}

}
