#include <bnp/graphics/mesh_factory.h>
#include <bnp/managers/render_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>
#include <bnp/components/physics.h>

#include <glm/glm.hpp>

#include <iostream>
using namespace std;

namespace bnp {

	RenderManager::RenderManager()
		: sprite_mesh(MeshFactory().box()),
		wireframe_material(MaterialFactory().wireframe_material())
	{
	}

	RenderManager::~RenderManager() {
		sprite_mesh.cleanup();
	}

	void RenderManager::render(const entt::registry& registry, const Renderer& renderer, const Camera& camera) {
		auto view = registry.view<Mesh, Material, Transform, Texture, Renderable>();

		for (auto entity : view) {
			if (!registry.all_of<Instances>(entity)) {
				auto& mesh = view.get<Mesh>(entity);
				auto& material = view.get<Material>(entity);
				auto& transform = view.get<Transform>(entity);
				auto& renderable = view.get<Renderable>(entity);
				auto& texture = view.get<Texture>(entity);

				if (renderable.value) {
					renderer.render(camera, mesh, material, texture, transform.world_transform);
				}
			}
		}
	}

	void RenderManager::render_wireframes(const entt::registry& registry, const Renderer& renderer, const Camera& camera) {
		glDisable(GL_DEPTH_TEST);

		// mesh/sprite wireframes
		{
			auto view = registry.view<Transform, Renderable>();
			glm::vec4 color(1.0f);

			for (auto entity : view) {
				auto& transform = view.get<Transform>(entity);

				if (registry.all_of<Mesh>(entity)) {
					auto& mesh = registry.get<Mesh>(entity);
					renderer.render_wireframe(camera, mesh, wireframe_material, transform.world_transform, color);
				}
				else {
					renderer.render_wireframe(camera, sprite_mesh, wireframe_material, transform.world_transform, color);
				}
			}
		}

		// physics body wireframes
		{
			auto view = registry.view<Renderable, PhysicsBody2D>();
			glm::vec4 color(0.0f, 0.0f, 1.0f, 1.0f);

			for (auto entity : view) {
				auto& body = view.get<PhysicsBody2D>(entity);
				auto& body_transform = body.body->GetTransform();
				auto& position = body_transform.p;
				auto& rotation = body_transform.q;

				b2PolygonShape* shape = static_cast<b2PolygonShape*>(body.body->GetFixtureList()->GetShape());

				// m_vertices[2] is top-right corner
				float height = shape->m_vertices[2].y * 2;
				float width = shape->m_vertices[2].x * 2;

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0));
				transform = glm::scale(transform, glm::vec3(width, height, 1.0f));

				renderer.render_wireframe(camera, sprite_mesh, wireframe_material, transform, color);
			}
		}

		glEnable(GL_DEPTH_TEST);
	}

	void RenderManager::render_instances(const entt::registry& registry, const Renderer& renderer, const Camera& camera) {
		auto view = registry.view<Mesh, Material, Renderable, Texture, Instances>();

		for (auto entity : view) {
			auto& mesh = view.get<Mesh>(entity);
			auto& material = view.get<Material>(entity);
			auto& instances = view.get<Instances>(entity);
			auto& renderable = view.get<Renderable>(entity);
			auto& texture = view.get<Texture>(entity);

			if (renderable.value) {
				const Transform& t = instances.transforms.at(0);
				renderer.render_instances(camera, mesh, material, texture, instances);
			}
		}

	}

	void RenderManager::render_sprites(const entt::registry& registry, const Renderer& renderer, const Camera& camera) {
		auto view = registry.view<Sprite, Material, Transform, Texture, Renderable>();

		for (auto entity : view) {
			auto& sprite = view.get<Sprite>(entity);
			auto& material = view.get<Material>(entity);
			auto& transform = view.get<Transform>(entity);
			auto& renderable = view.get<Renderable>(entity);
			auto& texture = view.get<Texture>(entity);

			// maybe todo: split into render animated vs static sprites, so we can use a ref here
			SpriteFrame sprite_frame;

			if (registry.all_of<SpriteAnimator>(entity)) {
				auto& animator = registry.get<SpriteAnimator>(entity);
				auto& animation = sprite.animations.at(animator.current_animation);

				sprite_frame = animation.frames.at(animator.current_frame_index);
			}
			else {
				sprite_frame = sprite.default_frame;
			}

			glDisable(GL_DEPTH_TEST);

			glm::vec3 scalar = glm::vec3(
				static_cast<float>(sprite.mirror.x),
				static_cast<float>(sprite.mirror.y),
				1.0f
			);
			glm::mat4 world_transform = transform.world_transform * glm::scale(glm::mat4(1.0f), scalar);

			if (renderable.value) {
				renderer.render_sprite(camera, sprite_frame, sprite_mesh, material, texture, world_transform);
			}

			glEnable(GL_DEPTH_TEST);
		}
	}

} // namespace bnp
