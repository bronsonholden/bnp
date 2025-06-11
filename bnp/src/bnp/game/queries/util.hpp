#pragma once

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Queries {

// Get the next available ID for the given component struct
template <typename T>
typename T::ID get_next_id(entt::registry& registry) {
	auto view = registry.view<T>();
	typename T::ID next_id = 1;

	for (auto entity : view) {
		auto& component = view.get<T>(entity);
		if (component.id >= next_id) next_id = component.id + 1;
	}

	return next_id;
}

}
}
}
