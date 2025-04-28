#include <bnp/factories/mesh_factory.h>
#include <bnp/managers/render_manager.h>
#include <bnp/components/behavior.h>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>
#include <bnp/components/physics.h>

#include <glm/glm.hpp>

#include <iostream>
using namespace std;

namespace bnp {

	RenderManager::RenderManager()
		: sprite_mesh(MeshFactory().box()),
		line_mesh(MeshFactory().line()),
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

	// todo: render instanced lines
	void RenderManager::render_flow_field_2ds(const entt::registry& registry, const Renderer& renderer, const Camera& camera) {
		glDisable(GL_DEPTH_TEST);

		auto view = registry.view<FlowField2D>();

		for (auto entity : view) {
			auto& field = view.get<FlowField2D>(entity);

			for (int y = 0; y < field.grid_size.y; ++y) {
				for (int x = 0; x < field.grid_size.x; ++x) {
					glm::vec2 dir = field.reverse_field.at(y * field.grid_size.x + x);
					if (glm::length(dir) < 0.001f) continue;
					float angle_rad = atan2(dir.x, dir.y);

					glm::vec2 cell_offset = glm::vec2(field.cell_size / 2, field.cell_size / 2);
					glm::vec2 position = field.origin + cell_offset + glm::vec2(x * field.cell_size, y * field.cell_size);

					glm::mat4 world_transform = glm::mat4(1.0f);
					world_transform = glm::translate(world_transform, glm::vec3(position, 0));
					glm::mat4 cell_world_transform = glm::scale(world_transform, glm::vec3(field.cell_size, field.cell_size, 1.0f));
					world_transform = glm::rotate(world_transform, angle_rad, glm::vec3(0, 0, -1));
					world_transform = glm::scale(world_transform, glm::vec3(1.0f, 0.1f, 1.0f));


					glm::vec4 color(0.0f, 1.0f, 0.0f, 1.0f);
					renderer.render_wireframe(camera, sprite_mesh, wireframe_material, cell_world_transform, color);
					renderer.render_line(camera, line_mesh, wireframe_material, world_transform);
				}
			}
		}


		glEnable(GL_DEPTH_TEST);
	}

	void RenderManager::render_wireframes(const entt::registry& registry, const Renderer& renderer, const Camera& camera) {
		glDisable(GL_DEPTH_TEST);

		// mesh/sprite wireframes
		{
			auto view = registry.view<Transform, Renderable>();
			glm::vec4 color(1.0f);

			for (auto entity : view) {
				if (!registry.any_of<Mesh, Sprite>(entity)) continue;

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

			if (!renderable.value) continue;

			for (auto& layer : sprite.layers) {
				if (!layer.visible) continue;

				// maybe todo: split into render animated vs static sprites, so we can use a ref here
				SpriteFrame sprite_frame;

				if (registry.all_of<SpriteAnimator>(entity)) {
					auto& animator = registry.get<SpriteAnimator>(entity);
					auto& animation = sprite.animations.at(animator.current_animation);

					// since all layers and frames are split out and listed in the sprite's frame
					// vector sequentially, we can render a different layer by applying an offset to
					// the canonical frame to render.
					//
					// todo: expand sprite layers to include the actual underlying frames so we can
					// directly render them
					uint32_t frame_index = animation.framelist.at(animator.current_framelist_index);
					sprite_frame = layer.frames.at(frame_index);
				}
				else {
					sprite_frame = sprite.layers.at(0).frames.at(0);
				}

				glDisable(GL_DEPTH_TEST);

				glm::vec3 scalar = glm::vec3(
					static_cast<float>(sprite.mirror.x),
					static_cast<float>(sprite.mirror.y),
					1.0f
				);
				glm::mat4 world_transform = transform.world_transform * glm::scale(glm::mat4(1.0f), scalar);

				renderer.render_sprite(camera, sprite, sprite_frame, sprite_mesh, material, texture, world_transform);

				glEnable(GL_DEPTH_TEST);
			}
		}
	}

	void RenderManager::render_water2d(const entt::registry& registry, const Renderer& renderer, const Camera& camera) {
		glm::vec4 color(0.678, 0.847, 0.902, 1.0);
		auto view = registry.view<Water2D, Material, Transform>();

		for (auto entity : view) {
			auto& water = view.get<Water2D>(entity);
			auto& transform = view.get<Transform>(entity);
			auto& material = view.get<Material>(entity);

			float half_width = water.columns * water.column_width * 0.5f;

			for (int i = 0; i < water.columns; ++i) {
				float x = -half_width + (i + 0.5f) * water.column_width;

				float surface_y = water.height[i];
				float height = surface_y;

				float center_y = height * 0.5f;

				glm::mat4 local_transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, center_y, 0.0f));
				local_transform = glm::scale(local_transform, glm::vec3(water.column_width, height, 1.0f));

				glm::mat4 world_transform = glm::translate(glm::mat4(1.0f), transform.position) * local_transform;

				renderer.render_wireframe(camera, sprite_mesh, material, world_transform, color, true);
			}
		}
	}

} // namespace bnp
