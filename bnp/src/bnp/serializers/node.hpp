#pragma once

#include <bnp/core/node.hpp>
#include <bnp/serializers/transform.hpp>
#include <bnp/serializers/graphics.hpp>

#include <iostream>
#include <type_traits>

using namespace std;


namespace bnp {

template<typename... Components>
void collect_component_ids(entt::registry& registry, entt::entity entity, std::vector<uint32_t>& out_ids) {
	((registry.all_of<Components>(entity) ? out_ids.push_back(entt::type_hash<Components>::value()) : void()), ...);
}

template <typename S>
void serialize(S& s, bnp::Node& n) {
	std::vector<uint32_t> component_ids;

	collect_component_ids<ALL_COMPONENTS>(n.registry, n.entity, component_ids);
	uint32_t num_components = component_ids.size();

	s.value4b(num_components);

	s.container(component_ids, num_components, [](auto& s, uint32_t& id) {
		s.value4b(id);
		});

	for (auto id : component_ids) {
		bool matched = false;

#define X(ComponentType) \
			if (!matched && id == entt::type_hash<ComponentType>::value()) { \
				if (n.registry.all_of<ComponentType>(n.entity)) { \
					ComponentType c = n.registry.get<ComponentType>(n.entity); \
					s.object(c); \
				} else { \
					ComponentType c; \
					s.object(c); \
					n.registry.emplace<ComponentType>(n.entity, c); \
				} \
				matched = true; \
			}

		REGISTERED_COMPONENTS
#undef X
	}
}
}

