#include <bnp/managers/render_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>

#include <glm/glm.hpp>

#include <iostream>
using namespace std;

namespace bnp {

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

} // namespace bnp
