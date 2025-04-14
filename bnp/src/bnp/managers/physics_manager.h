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
		void add_body(Node& node, const b2BodyDef& bodyDef, const b2FixtureDef& fixture_def);
		void destroy_body(b2Body* body);
		entt::entity get_entity_for_body(b2Body* body) const;
		b2World& get_world();

	private:
		b2World world;
	};

} // namespace bnp
