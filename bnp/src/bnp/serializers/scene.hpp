#pragma once

#include <bnp/core/scene.h>
#include <bnp/serializers/node.hpp>
#include <bnp/serializers/graphics.hpp>
#include <bnp/serializers/transform.hpp>

#include <bitsery/traits/vector.h>

#include <iostream>
using namespace std;

namespace bnp {

template <typename S>
void serialize(S& s, Scene& scene) {
	auto& registry = scene.get_registry();
	auto& nodes = scene.get_nodes();

	const bool serializing = nodes.size() > 0;

	// Serialize node count
	uint32_t node_count = static_cast<uint32_t>(nodes.size());

	s.value4b(node_count);

	if (serializing) {
		for (auto& [_, node] : nodes) {
			s.object(node);
		}
	}
	else {
		for (uint32_t i = 0; i < node_count; ++i) {
			bnp::Node node{ registry };
			s.object(node);
			scene.emplace_node(node);
		}

	}
}

}
