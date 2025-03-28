#include <iostream>
#include "testing.h"
#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>

TEST_CASE("tracks number of components") {
	entt::registry registry;
	bnp::Node node(registry);

	node.add_component<int>(1);
	REQUIRE(node.get_num_components() == 1);

	node.remove_component<int>();
	REQUIRE(node.get_num_components() == 0);
}
