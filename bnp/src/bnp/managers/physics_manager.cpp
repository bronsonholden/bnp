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

			glm::vec2 distance{ pos.x - transform.position.x, pos.y - transform.position.y };

			if (glm::length(distance) > 0.001f) {
				registry.patch<Transform>(entity, [&pos](Transform& t) {
					t.position.x = pos.x;
					t.position.y = pos.y;
					t.dirty = true;
					});
			}
		}
	}

	void PhysicsManager::add_body(Node& node, const b2BodyDef& body_def, const b2FixtureDef& fixture_def) {
		b2Body* body = world.CreateBody(&body_def);

		body->CreateFixture(&fixture_def);
		body->GetUserData().pointer = static_cast<uintptr_t>(node.get_entity_id());

		node.add_component<PhysicsBody2D>(PhysicsBody2D{ body });
	}

	void PhysicsManager::generate_sprite_bodies(entt::registry& registry) {
		auto view = registry.view<Sprite, Transform>();

		for (auto entity : view) {
			auto& sprite = view.get<Sprite>(entity);

			if (sprite.slices.find("Body") == sprite.slices.end()) continue;

			auto& body_slice = sprite.slices.at("Body");
			auto& coords = body_slice.at(0);

			// zero-size body
			if (!coords.z || !coords.w) continue;

			auto& transform = view.get<Transform>(entity);

			float half_width = static_cast<float>(coords.z) / (sprite.frame_width * 2);
			float half_height = static_cast<float>(coords.w) / (sprite.frame_height * 2);

			b2BodyDef body_def;
			b2PolygonShape body_shape;
			body_shape.SetAsBox(half_width, half_height);

			b2FixtureDef fixture_def;
			fixture_def.shape = &body_shape;
			fixture_def.density = 1.0f;
			fixture_def.friction = 0.3f;
			fixture_def.restitution = 0.1f;

			body_def.type = b2_dynamicBody;
			body_def.awake = true;
			body_def.enabled = true;
			body_def.position = b2Vec2(transform.position.x, transform.position.y);
			body_def.gravityScale = 0.0f; // fake ground

			b2Body* body = world.CreateBody(&body_def);

			body->CreateFixture(&fixture_def);
			body->GetUserData().pointer = static_cast<uintptr_t>(entity);

			registry.emplace<PhysicsBody2D>(entity, PhysicsBody2D{ body });
		}

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
