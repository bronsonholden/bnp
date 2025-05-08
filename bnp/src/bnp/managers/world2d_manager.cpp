#include <bnp/managers/world2d_manager.h>
#include <bnp/components/physics.h>

namespace bnp {

	World2DManager::World2DManager() {

	}

	World2DManager::~World2DManager() {

	}

	void World2DManager::update(entt::registry& registry, float dt) {
		auto& global = registry.get<Global>(registry.view<Global>().front());
		b2World& physics_world = *global.world;

		auto view = registry.view<World2D, Tilemap>();

		for (auto entity : view) {
			auto& world = view.get<World2D>(entity);

			if (world.init) continue;

			auto& tilemap = view.get<Tilemap>(entity);

			auto rects = tilemap.decompose();


			// todo: lazy load rects by taking a slice of of the tilemap data for a
			// given partition.
			// todo: when creating the entity, it needs a World2DChunk component or
			// similar so we can query for it and disable it when the partition is
			// not active (likely will go with viewer's current partition and any
			// neighboring partitions for now)
			for (auto rect : rects) {
				entt::entity entity = registry.create();

				int flipped_y = tilemap.height - rect.y - rect.w;

				float tile_size = world.tile_size;

				float center_x = (rect.x + rect.z / 2.0f) * tile_size;
				float center_y = (flipped_y + rect.w / 2.0f) * tile_size;

				float half_width = (rect.z * tile_size) / 2.0f;
				float half_height = (rect.w * tile_size) / 2.0f;

				b2BodyDef body_def;
				body_def.type = b2_staticBody;
				body_def.position.Set(center_x, center_y);
				b2Body* body = physics_world.CreateBody(&body_def);

				b2PolygonShape box;
				box.SetAsBox(half_width, half_height);

				b2FixtureDef fixture_def;
				fixture_def.shape = &box;
				fixture_def.friction = 0.8f;
				body->CreateFixture(&fixture_def);

				registry.emplace<Transform>(entity, Transform{});
				registry.emplace<PhysicsBody2D>(entity, PhysicsBody2D{ body });
				registry.emplace<Renderable>(entity, true);
			}

			registry.patch<World2D>(entity, [](World2D& w) {
				w.init = true;
				});
		}
	}

}
