// Special serializers for registry views, especially useful
// for static game data or save files.
//
// Of the components passed to the template class list, those
// not wrapped in an `Optional` template struct will be used
// to create the view. Only entities in that view will be
// serialized or deserialized. The optional components will
// only be serialized if the entity has them.

#pragma once

#include <bnp/core/logger.hpp>

#include <entt/entt.hpp>
#include <tuple>
#include <type_traits>
#include <utility>
#include <string>

template <typename T>
constexpr std::string type_name() {
#if defined(__clang__)
	constexpr std::string p = __PRETTY_FUNCTION__;
	return p.substr(31, p.size() - 31 - 1);
#elif defined(__GNUC__)
	constexpr std::string p = __PRETTY_FUNCTION__;
	return p.substr(46, p.size() - 46 - 1);
#elif defined(_MSC_VER)
	std::string p = __FUNCSIG__;
	return p.substr(118, p.size() - 118 - 7); // custom trim
#else
	return "unknown";
#endif
}

namespace bnp {

// This struct marks the component as optional. It will be serialized if present
template <typename T>
struct Optional {
	using type = T;
};

template <typename T>
struct is_optional : std::false_type {};

template <typename U>
struct is_optional<Optional<U>> : std::true_type {};

template <typename T>
struct unwrap_optional {
	using type = T;
};

template <typename U>
struct unwrap_optional<Optional<U>> {
	using type = U;
};

// The structs/types and tuple-related functions below enable filtering
// optional components out, so the rest can be used in creating registry views.
template <typename... Ts>
struct filter_mandatory;

template <typename T, typename... Rest>
struct filter_mandatory<T, Rest...> {
private:
	using Tail = typename filter_mandatory<Rest...>::type;
public:
	using type = std::conditional_t<
		is_optional<T>::value,
		Tail,
		decltype(std::tuple_cat(std::declval<std::tuple<T>>(), std::declval<Tail>()))
	>;
};

template <>
struct filter_mandatory<> {
	using type = std::tuple<>;
};

template <typename... Ts>
using filter_mandatory_t = typename filter_mandatory<Ts...>::type;

template <typename F, typename Tuple, std::size_t... I>
void apply_tuple_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
	f.template operator() < std::tuple_element_t<I, std::decay_t<Tuple>>... > ();
}

template <typename F, typename Tuple>
void apply_tuple(F&& f, Tuple&& t) {
	constexpr std::size_t N = std::tuple_size<std::decay_t<Tuple>>::value;
	apply_tuple_impl(std::forward<F>(f), std::forward<Tuple>(t), std::make_index_sequence<N>{});
}

template <typename S, typename T>
void serialize_component(S& s, entt::registry& registry, entt::entity entity) {
	using ActualT = typename unwrap_optional<T>::type;

	if constexpr (!is_optional<T>::value) {
		using get_result_t = decltype(registry.template get<ActualT>(entity));
		if constexpr (!std::is_void_v<get_result_t>) {
			auto& component = registry.template get<ActualT>(entity);
			s.object(component);
		}
	}
	else {
		using get_result_t = decltype(registry.template get<ActualT>(entity));
		uint8_t presence = registry.all_of<ActualT>(entity) ? 1 : 0;
		s.value1b(presence);
		if constexpr (!std::is_void_v<get_result_t>) {
			if (presence) {
				auto& component = registry.template get<ActualT>(entity);
				s.object(component);
			}
		}
	}
	Log::info("  Serialized %s", type_name<T>().c_str());
}

template <typename S, typename... Components>
void serialize(S& s, entt::registry& registry, uint32_t version) {
	using mandatory_components = filter_mandatory_t<Components...>;

	apply_tuple([&]<typename... Mandatory>() {
		auto view = registry.view<Mandatory...>();

		std::vector<entt::entity> entities;
		for (auto entity : view) {
			entities.push_back(entity);
		}
		std::sort(entities.begin(), entities.end());

		uint64_t count = entities.size();

		Log::info("Serializing %d entities", count);

		s.value4b(version);
		s.value8b(count);

		for (auto entity : entities) {
			Log::info("Serializing entity %d", (int)entity);
			(serialize_component<S, Components>(s, registry, entity), ...);
		}
	}, mandatory_components{});
}

template <typename S, typename T>
void deserialize_component(S& s, entt::registry& registry, entt::entity entity) {
	using ActualT = typename unwrap_optional<T>::type;
	using get_result_t = decltype(registry.template get<ActualT>(entity));

	if constexpr (!is_optional<T>::value) {
		if constexpr (!std::is_void_v<get_result_t>) {
			ActualT component{};
			s.object(component);
			registry.emplace<ActualT>(entity, component);
		}
		else {
			registry.emplace<ActualT>(entity);
		}
	}
	else {
		uint8_t presence;
		s.value1b(presence);

		if (presence) {
			if constexpr (!std::is_void_v<get_result_t>) {
				ActualT component{};
				s.object(component);
				registry.emplace<ActualT>(entity, component);
			}
			else {
				registry.emplace<ActualT>(entity);
			}
		}
	}

	Log::info("  Deserialized %s", type_name<T>().c_str());
}

template <typename S, typename... Components>
uint32_t deserialize(S& s, entt::registry& registry) {
	uint32_t version;
	uint64_t count;

	s.value4b(version);
	s.value8b(count);

	for (uint64_t i = 0; i < count; ++i) {
		entt::entity entity = registry.create();
		Log::info("Deserializing into entity %d", (int)entity);
		(deserialize_component<S, Components>(s, registry, entity), ...);
	}

	return version;
}

}
