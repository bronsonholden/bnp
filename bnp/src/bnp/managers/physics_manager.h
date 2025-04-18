#pragma once

#include <bnp/core/node.hpp>
#include <bnp/components/physics.h>

#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include <unordered_map>

namespace bnp {

	class PhysicsManager {
	public:
		PhysicsManager();

		void update(entt::registry& registry, float dt);

		// for all sprites in the registry without a physics component, generate one
		void generate_sprite_bodies(entt::registry& registry);

		void add_body(Node& node, const b2BodyDef& bodyDef, const b2FixtureDef& fixture_def);
		void destroy_body(b2Body* body);
		entt::entity get_entity_for_body(b2Body* body) const;
		b2World& get_world();

	private:
		b2World world;

		void update_water2d_collisions(entt::registry& registry, float dt);
	};

} // namespace bnp
