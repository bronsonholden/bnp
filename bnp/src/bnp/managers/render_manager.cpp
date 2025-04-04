#include <bnp/managers/render_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>

#include <glm/glm.hpp>

#include <iostream>
using namespace std;

namespace bnp {

	void RenderManager::render(const entt::registry& registry, const Renderer& renderer) {
		auto view = registry.view<Mesh, Material, Transform, Renderable>();

		// todo: accept as arg
		Camera camera({
			glm::vec3(5.0f, 5.0f, 5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			});

		for (auto entity : view) {
			if (!registry.all_of<Instances>(entity)) {
				auto& mesh = view.get<Mesh>(entity);
				auto& material = view.get<Material>(entity);
				auto& transform = view.get<Transform>(entity);
				auto renderable = view.get<Renderable>(entity);

				if (renderable.value) {
					renderer.render(camera, mesh, material, transform.world_transform);
				}
			}
		}
	}

	void RenderManager::render_instances(const entt::registry& registry, const Renderer& renderer) {
		auto view = registry.view<Mesh, Material, Renderable, Instances>();

		//cout << "render_instances (registry size): " << registry.size() << endl;
		//cout << "render_instances (view size): " << view.size_hint() << endl;

		// todo: accept as arg
		Camera camera({
			glm::vec3(5.0f, 5.0f, 5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			});

		for (auto entity : view) {
			auto& mesh = view.get<Mesh>(entity);
			auto& material = view.get<Material>(entity);
			auto& instances = view.get<Instances>(entity);
			auto& renderable = view.get<Renderable>(entity);

			if (renderable.value) {
				renderer.render_instances(camera, mesh, material, instances);
			}
		}
	}

} // namespace bnp
