#include <bnp/core/node.hpp>

namespace bnp {
	Node::Node(const Node& other)
		: registry(other.registry),
		entity(other.entity),
		num_components(other.num_components),
		valid(other.valid)
	{
	}

	Node::Node(entt::registry& _registry, entt::entity _entity)
		: registry(_registry),
		entity(_entity),
		num_components(0)
	{
		valid = registry.valid(entity);
	}

	Node::Node(entt::registry& _registry)
		: Node(_registry, _registry.create())
	{
	}

	Node::~Node() {
		registry.destroy(entity);
	}

	unsigned int Node::get_num_components() const {
		return num_components;
	}


	Node Node::create_child() {
		Node c(registry);
		return attach_child(c);
	}

	Node Node::attach_child(Node n) {
		registry.emplace<Parent>(n.entity, entity);
		return n;
	}

	entt::registry& Node::get_registry() const { return registry; }
	entt::entity Node::get_entity_id() const { return entity; }
}
