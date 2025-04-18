#include <bnp/managers/physics_manager.h>
#include <bnp/components/transform.h>

namespace bnp {

	PhysicsManager::PhysicsManager()
		: world(b2Vec2(0.0f, -9.8))
	{
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0.0f, -0.5f);  // Set Y to your desired "floor"
		groundBodyDef.type = b2_staticBody;

		b2Body* groundBody = world.CreateBody(&groundBodyDef);

		// Create a box shape for the ground
		b2PolygonShape groundBox;
		groundBox.SetAsBox(50.0f, 0.5f);  // Width, Height (flat platform)

		// Attach the shape to the ground body
		groundBody->CreateFixture(&groundBox, 0.0f);
	}

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

		update_water2d_collisions(registry, dt);
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
			fixture_def.density = 5.0f;
			fixture_def.friction = 0.3f;
			fixture_def.restitution = 0.0f;

			body_def.type = b2_dynamicBody;
			body_def.awake = true;
			body_def.enabled = true;
			body_def.position = b2Vec2(transform.position.x, transform.position.y);
			body_def.gravityScale = 1.0f;

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

	void PhysicsManager::update_water2d_collisions(entt::registry& registry, float dt) {
		auto view = registry.view<Water2D, Transform>();
		auto bodies = registry.view<PhysicsBody2D>();

		for (auto entity : view) {
			auto& water = view.get<Water2D>(entity);
			auto& water_transform = view.get<Transform>(entity); // world-space position of water

			float water_origin_x = water_transform.position.x;
			float water_origin_y = water_transform.position.y;
			float half_width = (water.columns * water.column_width) * 0.5f;

			float water_left = water_origin_x - half_width;

			for (auto body_entity : bodies) {
				auto& body = bodies.get<PhysicsBody2D>(body_entity);
				float body_y_vel = std::abs(body.body->GetLinearVelocity().y);
				b2Fixture* fixture = body.body->GetFixtureList();
				if (!fixture) continue;

				b2AABB aabb = fixture->GetAABB(0);

				// AABB min/max in world coordinates
				float aabb_min_x = aabb.lowerBound.x;
				float aabb_max_x = aabb.upperBound.x;
				float aabb_min_y = aabb.lowerBound.y;
				float aabb_max_y = aabb.upperBound.y;
				float aabb_center_y = aabb_min_y + ((aabb_max_y - aabb_min_y) / 2);

				// Convert AABB x-range to column indices
				int start_col = static_cast<int>((aabb_min_x - water_left) / water.column_width);
				int end_col = static_cast<int>((aabb_max_x - water_left) / water.column_width);

				// Clamp column range to valid range
				start_col = std::max(0, start_col);
				end_col = std::min(water.columns - 1, end_col);

				for (int i = start_col - 3; i < start_col; ++i) {
					float surface_y = water.height[i] + water_origin_y;

					if (aabb_center_y < surface_y) {
						water.velocity[i] += 0.1f * dt * (1 + body_y_vel * 2);
					}
				}

				for (int i = end_col + 1; i < end_col + 3; ++i) {
					float surface_y = water.height[i] + water_origin_y;

					if (aabb_center_y < surface_y) {
						water.velocity[i] += 0.1f * dt * (1 + body_y_vel * 2);
					}
				}

				// Apply splash force to intersecting columns
				for (int i = start_col; i <= end_col; ++i) {
					float surface_y = water.height[i] + water_origin_y;

					if (aabb_center_y < surface_y) {
						water.velocity[i] += -0.1f * dt * (1 + body_y_vel * 2);
					}
				}
			}
		}
	}

}
