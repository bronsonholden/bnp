#include <entt/entt.hpp>
#include "testing.h"
#include <bnp/components/transform.h>

TEST_CASE("emplace position") {
	entt::registry registry;
	entt::entity entity = registry.create();

	registry.emplace<bnp::Position>(entity, bnp::Position{ { 1.0f, 1.0f, 1.0f } });

	CHECK(registry.all_of<bnp::Position>(entity));
}
