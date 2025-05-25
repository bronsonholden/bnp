#include <bnp/managers/planet_2d_manager.h>

#include <bnp/components/world.h>

namespace bnp {

	void Planet2DManager::update(entt::registry& registry, float dt) {
		for (auto entity : registry.view<Planet2D>()) {
			registry.patch<Planet2D>(entity, [=](Planet2D& p) {
				p.time += dt;
				});
		}
	}

}
