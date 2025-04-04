#include <entt/entt.hpp>
#include "testing.h"
#include <bnp/components/transform.h>

TEST_CASE("emplace transform") {
	entt::registry registry;
	entt::entity entity = registry.create();

	registry.emplace<bnp::Transform>(entity, bnp::Transform{ { 1.0f, 1.0f, 1.0f } });

	CHECK(registry.all_of<bnp::Transform>(entity));
}
