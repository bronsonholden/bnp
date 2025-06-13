// Special serializers for registry views, especially useful
// for static game data or save files.

#pragma once

#include <entt/entt.hpp>

namespace bnp {

template <typename S, typename V, typename T>
void serialize_component_if_not_void(S& s, V& view, entt::entity entity) {
	using get_result_t = decltype(view.template get<T>(entity));

	if constexpr (!std::is_void_v<get_result_t>) {
		auto& component = view.template get<T>(entity);
		s.object(component);
	}
}

template <typename S, typename ...Components>
void serialize(S& s, entt::registry& registry, uint32_t version) {
	auto view = registry.view<Components...>();

	uint64_t count = 0;
	for (auto entity : view) ++count;

	s.value4b(version);
	s.value8b(count);

	for (auto entity : view) {
		(serialize_component_if_not_void<S, decltype(view), Components>(s, view, entity), ...);
	}
}

template <typename S, typename T>
void deserialize_component_if_not_void(S& s, entt::registry& registry, entt::entity entity) {
	using get_result_t = decltype(registry.template get<T>(entity));

	if constexpr (!std::is_void_v<get_result_t>) {
		T component{};
		s.object(component);
		registry.emplace<T>(entity, component);
	}
	else {
		registry.emplace<T>(entity);
	}
}

template <typename S, typename ...Components>
auto deserialize(S& s, entt::registry& registry, uint32_t* out_version = nullptr) {
	uint32_t version;
	uint64_t count;

	s.value4b(version);
	s.value8b(count);

	for (uint64_t i = 0; i < count; ++i) {
		entt::entity entity = registry.create();
		(deserialize_component_if_not_void<S, Components>(s, registry, entity), ...);
	}

	if (out_version) *out_version = version;

	return registry.view<Components...>();
}

template <typename S, typename ...Components>
uint32_t deserialize(S& s, entt::registry& registry) {
	entt::entity entity = registry.create();
	uint32_t version;

	s.value4b(version);
	(deserialize_component_if_not_void<S, Components>(s, registry, entity), ...);

	return version;
}

}
