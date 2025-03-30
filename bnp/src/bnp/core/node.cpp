#include <bnp/core/node.hpp>

#include <iostream>
using namespace std;

namespace bnp {
	template<typename... Components>
	unsigned int count_components(entt::registry& registry, entt::entity entity) {
		return (static_cast<unsigned int>(registry.all_of<Components>(entity)) + ...);
	}

	Node::Node(const Node& other)
		: registry(other.registry),
		entity(other.entity)
	{
	}

	Node::Node(entt::registry& _registry, entt::entity _entity)
		: registry(_registry),
		entity(_entity)
	{
	}

	Node::Node(entt::registry& _registry)
		: Node(_registry, _registry.create())
	{
	}

	Node::~Node() {
	}

	unsigned int Node::get_num_components() const {
		return count_components<ALL_COMPONENTS>(registry, entity);
	}

	Node Node::create_child() {
		Node c(registry);
		return attach_child(c);
	}

	Node Node::attach_child(Node n) {
		registry.emplace<Parent>(n.entity, entity);
		return n;
	}

	entt::registry& Node::get_registry() const {
		return registry;
	}

	entt::entity Node::get_entity_id() const {
		return entity;
	}
}
