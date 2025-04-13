#include <bnp/graphics/mesh_factory.h>
#include <bnp/managers/render_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>

#include <glm/glm.hpp>

#include <iostream>
using namespace std;

namespace bnp {

	RenderManager::RenderManager()
		: sprite_mesh(MeshFactory().box())
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

			SpriteFrame sprite_frame;

			if (registry.all_of<SpriteAnimator>(entity)) {
				auto& animator = registry.get<SpriteAnimator>(entity);
				auto& animation = sprite.animations.at(animator.current_animation);

				sprite_frame = animation.frames.at(animator.current_frame_index);
			}
			else {
				sprite_frame.frame_index = 0;
				sprite_frame.uv0 = { 0, 0 };
				sprite_frame.uv1 = {
					static_cast<float>(sprite.frame_width) / sprite.spritesheet_width,
					static_cast<float>(sprite.frame_height) / sprite.spritesheet_height
				};
			}

			glDisable(GL_DEPTH_TEST);

			if (renderable.value) {
				renderer.render_sprite(camera, sprite_frame, sprite_mesh, material, texture, transform.world_transform);
			}

			glEnable(GL_DEPTH_TEST);
		}
	}

} // namespace bnp
