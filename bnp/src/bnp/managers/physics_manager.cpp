#include <bnp/core/logger.hpp>
#include <bnp/managers/physics_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/global.h>

#include <iostream>
using namespace std;

namespace bnp {

void on_physics_body_2d_destroy(entt::registry& registry, entt::entity entity) {
	auto& body = registry.get<PhysicsBody2D>(entity);

	body.body->GetWorld()->DestroyBody(body.body);

	Log::info("Destroying Box2D body 0x%0X", body.body);
}

PhysicsManager::PhysicsManager(entt::registry& registry)
	: world(b2Vec2(0.0f, -14.6))
{
	registry.on_destroy<PhysicsBody2D>().connect<&on_physics_body_2d_destroy>();
}

PhysicsManager::~PhysicsManager() {
	cout << "Shutting down PhysicsManager" << endl;
}

void PhysicsManager::update(entt::registry& registry, float dt) {
	const int velocity_iterations = 8;
	const int position_iterations = 3;

	world.Step(dt, velocity_iterations, position_iterations);

	auto view = registry.view<PhysicsBody2D, Transform>();

	for (auto entity : view) {
		auto& body = registry.get<PhysicsBody2D>(entity);
		auto& transform = registry.get<Transform>(entity);

		const b2Vec2& pos = body.body->GetPosition();

		registry.patch<Transform>(entity, [&](Transform& t) {
			glm::mat4 world_transform =
				glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0)) *
				glm::mat4_cast(t.rotation) *
				glm::scale(glm::mat4(1.0f), t.scale);
			t.world_transform = world_transform;
			t.dirty = false;
			});
	}

	update_water2d_collisions(registry, dt);
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

		// todo: we shouldn't auto generate bodies, so this first-layer-first-frame hack works for now
		float half_width = static_cast<float>(coords.z) / (sprite.layers.at(0).frames.at(0).size.x * 2);
		float half_height = static_cast<float>(coords.w) / (sprite.layers.at(0).frames.at(0).size.y * 2);

		b2BodyDef body_def;
		b2PolygonShape body_shape;
		body_shape.SetAsBox(half_width, half_height);

		b2FixtureDef fixture_def;
		fixture_def.shape = &body_shape;
		fixture_def.density = 5.0f;
		fixture_def.friction = 0.0f;
		fixture_def.restitution = 0.0f;

		body_def.type = b2_dynamicBody;
		body_def.awake = true;
		body_def.enabled = true;
		body_def.position = b2Vec2(transform.position.x, transform.position.y);
		body_def.gravityScale = 1.0f;
		body_def.fixedRotation = true;

		b2Body* body = world.CreateBody(&body_def);

		body->CreateFixture(&fixture_def);
		body->GetUserData().pointer = static_cast<uintptr_t>(entity);

		registry.emplace<PhysicsBody2D>(entity, PhysicsBody2D{ body });
	}

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
		// todo: make sure we get surface height in world space

		float water_origin_x = water_transform.position.x;
		float water_origin_y = water_transform.position.y;
		float half_width = (water.columns * water.column_width) * 0.5f;

		float water_left = water_origin_x - half_width;

		for (auto body_entity : bodies) {
			auto& body = bodies.get<PhysicsBody2D>(body_entity);
			b2Vec2 velocity = body.body->GetLinearVelocity();
			// less emphasis on horizontal movement
			float speed = glm::length(glm::vec2(velocity.x / 2, velocity.y));
			b2Fixture* fixture = body.body->GetFixtureList();
			if (!fixture || fixture->IsSensor()) continue;

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

			if (velocity.x < 0) {
				for (int i = start_col - 3; i >= 0 && i < water.columns && i < start_col; ++i) {
					float surface_y = water.height[i] + water_origin_y;

					if (aabb_min_y < surface_y) {
						water.velocity[i] += 0.1f * dt * speed * 0.5f;
					}
				}
			}
			else if (velocity.x > 0) {
				for (int i = end_col + 1; i >= 0 && i < water.columns && i < end_col + 3; ++i) {
					float surface_y = water.height[i] + water_origin_y;

					if (aabb_min_y < surface_y) {
						water.velocity[i] += 0.1f * dt * speed * 0.5f;
					}
				}
			}


			// Apply splash force to intersecting columns
			for (int i = start_col; i <= end_col; ++i) {
				float surface_y = water.height[i] + water_origin_y;

				if (aabb_center_y < surface_y && aabb_max_y > surface_y) {
					water.velocity[i] += -0.1f * dt * (0.1 + velocity.y);
				}

				if (aabb_max_y < surface_y && (surface_y - aabb_max_y) < ((aabb_max_y - aabb_min_y) / 2)) {
					water.velocity[i] += 0.1f * dt * (0.1 + velocity.y);
				}
			}
		}
	}

}
}
