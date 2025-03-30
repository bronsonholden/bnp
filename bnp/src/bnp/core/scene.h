#pragma once

#include <bnp/core/node.hpp>

#include <unordered_map>
#include <entt/entt.hpp>

namespace bnp {

	class Scene {
	public:
		Scene(entt::registry& registry);
		~Scene();

		void remove_node(Node node);
		Node create_node();
		Node get_node(entt::entity entity);
		void emplace_node(Node n);
		std::unordered_map<entt::entity, Node>& get_nodes();

		entt::registry& get_registry();

		std::vector<Node> get_all_nodes();

	private:
		entt::registry& registry;
		std::unordered_map<entt::entity, Node> nodes;
	};

}
