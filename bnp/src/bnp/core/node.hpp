#pragma once

#include <entt/entt.hpp>
#include <bnp/components.h>
#include <bnp/serializers/transform.hpp>

namespace bnp {
	class Node {
	public:
		Node(const Node& other);
		Node(entt::registry& registry);
		Node(entt::registry& registry, entt::entity entity);
		~Node();

		template<typename T, typename... Args>
		const T& add_component(Args&&... args) {
			const T& component = registry.emplace<T>(entity, std::forward<Args>(args)...);
			num_components += 1;
			return component;
		}

		template<typename T>
		void remove_component() {
			registry.remove<T>(entity);
			num_components -= 1;
		}


		unsigned int get_num_components() const;

		template <typename S>
		friend void serialize(S& s, Node& n);

		Node create_child();
		Node attach_child(Node n);

		entt::registry& get_registry() const;
		entt::entity get_entity_id() const;

	protected:
		entt::registry& registry;
		entt::entity entity;

		unsigned int num_components;
		bool valid;
	};
}
