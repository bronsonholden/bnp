#include <iostream>
#include "testing.h"
#include <bnp/core/scene.h>

TEST_CASE("emplacing duplicate node is idempotent") {
	entt::registry registry;
	bnp::Scene scene(registry);

	REQUIRE(scene.get_nodes().size() == 0);
	bnp::Node node = scene.create_node();
	entt::entity entity = node.get_entity_id();
	REQUIRE(scene.get_nodes().size() == 1);

	REQUIRE(node.valid());
	REQUIRE(node.get_entity_id() == entity);

	scene.emplace_node(node);
	REQUIRE(scene.get_nodes().size() == 1);

	REQUIRE(node.valid());
	REQUIRE(node.get_entity_id() == entity);
}
