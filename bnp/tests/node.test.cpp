#include <iostream>
#include "testing.h"
#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>

TEST_CASE("tracks number of components") {
	entt::registry registry;
	bnp::Node node(registry);

	node.add_component<bnp::Transform>(bnp::Transform{ { 1.0f, 1.0f, 1.0f } });
	REQUIRE(node.get_num_components() == 1);

	node.remove_component<bnp::Transform>();
	REQUIRE(node.get_num_components() == 0);
}

TEST_CASE("copies retain components") {
	entt::registry registry;
	bnp::Node node(registry);
	bnp::Node copy = node;

	node.add_component<bnp::Transform>(bnp::Transform{ { 1.0f, 1.0f, 1.0f } });
	REQUIRE(node.get_num_components() == 1);
	REQUIRE(copy.get_num_components() == 1);
	REQUIRE(node.get_entity_id() == copy.get_entity_id());

}
