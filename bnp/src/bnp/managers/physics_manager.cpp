#include <bnp/managers/physics_manager.h>
#include <bnp/components/transform.h>

namespace bnp {

	PhysicsManager::PhysicsManager()
		: world(b2Vec2(0.0f, -9.8))
	{}

	void PhysicsManager::update(entt::registry& registry, float dt) {
		const int velocityIterations = 1;
		const int positionIterations = 1;

		world.Step(dt, velocityIterations, positionIterations);

		auto view = registry.view<PhysicsBody2D, Transform>();

		for (auto entity : view) {
			auto& body = registry.get<PhysicsBody2D>(entity);
			auto& transform = registry.get<Transform>(entity);

			const b2Vec2& pos = body.body->GetPosition();

			registry.patch<Transform>(entity, [&pos](Transform& t) {
				t.position.x = pos.x;
				t.position.y = pos.y;
				t.update_world_transform();
				});
		}
	}

	void PhysicsManager::add_body(Node& node, const b2BodyDef& body_def, const b2FixtureDef& fixture_def) {
		b2Body* body = world.CreateBody(&body_def);

		body->CreateFixture(&fixture_def);
		body->GetUserData().pointer = static_cast<uintptr_t>(node.get_entity_id());

		node.add_component<PhysicsBody2D>(PhysicsBody2D{ body });
	}

	void PhysicsManager::destroy_body(b2Body* body) {
		world.DestroyBody(body);
	}

	entt::entity PhysicsManager::get_entity_for_body(b2Body* body) const {
		return static_cast<entt::entity>(body->GetUserData().pointer);
	}

	b2World& PhysicsManager::get_world() {
		return world;
	}

}
