#include <bnp/core/scene.h>

#include <iostream>

namespace bnp {

Scene::Scene(entt::registry& _registry) : registry(_registry) {

}

Scene::~Scene() {
	for (auto [entity, _] : nodes) {
		registry.destroy(entity);
	}
}

Node Scene::create_node() {
	entt::entity e = registry.create();
	Node node(registry, e);
	nodes.emplace(e, node);
	return node;
}

Node Scene::get_node(entt::entity entity) {
	return nodes.at(entity);
}

void Scene::emplace_node(Node n) {
	nodes.emplace(n.get_entity_id(), n);
}

void Scene::remove_node(Node n) {
	nodes.erase(n.get_entity_id());
}

std::unordered_map<entt::entity, Node>& Scene::get_nodes() {
	return nodes;
}

entt::registry& Scene::get_registry() {
	return registry;
}

std::vector<Node> Scene::get_all_nodes() {
	std::vector<Node> result;

	result.reserve(nodes.size());

	for (auto& [entity, node] : nodes) {
		result.push_back(node);
	}

	return result;
}

}
