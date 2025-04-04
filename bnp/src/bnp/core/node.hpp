#pragma once

#include <bnp/components.h>
#include <bnp/serializers/transform.hpp>

#include <entt/entt.hpp>
#include <iostream>

namespace bnp {

	class Node {
	public:
		Node(const Node& other);
		Node(entt::registry& registry);
		Node(entt::registry& registry, entt::entity entity);
		~Node();

		bool valid() {
			return registry.valid(entity);
		}

		template<typename T, typename... Args>
		const T& add_component(Args&&... args) {
			const T& component = registry.emplace<T>(entity, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		bool has_component() const {
			return registry.all_of<T>(entity);
		}

		template<typename T>
		T& get_component() const {
			return registry.get<T>(entity);
		}

		template<typename T>
		void remove_component() {
			registry.remove<T>(entity);
		}

		template<>
		void remove_component<Instances>() {
			if (has_component<Instances>()) {
				Instances& i = get_component<Instances>();
				i.cleanup();
			}
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
	};

}
