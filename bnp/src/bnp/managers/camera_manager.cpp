#include <bnp/managers/camera_manager.h>
#include <bnp/components/graphics.h>

namespace bnp {

void CameraManager::update(entt::registry& registry, float dt) {
	auto view = registry.view<Camera2DRig, Transform>();

	for (auto entity : view) {
		auto& transform = registry.get<Transform>(entity);

		registry.patch<Camera2DRig>(entity, [&](Camera2DRig& rig) {
			glm::vec2 diff = rig.camera_worldspace_position - glm::vec2(transform.world_transform[3]);

			diff *= rig.auto_center_impulse;

			if (glm::length(diff) < 0.001f) diff = glm::vec2(0.0);

			if (diff.y > rig.pan_limit.y) {
				diff.y = rig.pan_limit.y;

			}
			else if (diff.y < -rig.pan_limit.y) {
				diff.y = -rig.pan_limit.y;
			}

			if (diff.x > rig.pan_limit.x) {
				diff.x = rig.pan_limit.x;

			}
			else if (diff.x < -rig.pan_limit.x) {
				diff.x = -rig.pan_limit.x;
			}

			rig.camera_worldspace_position = glm::vec2(transform.world_transform[3]) + diff;
			});
	}
}

}
