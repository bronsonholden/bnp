#include <bnp/managers/render_manager.h>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>

#include <glm/glm.hpp>

namespace bnp {

	void RenderManager::render(const entt::registry& registry, const Renderer& renderer) {
		auto view = registry.view<Mesh, Material, Position, Renderable>();

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
				auto& position = view.get<Position>(entity);
				auto renderable = view.get<Renderable>(entity);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), position.value);

				if (renderable) {
					renderer.render(camera, mesh, material, transform);
				}
			}
		}
	}

	void RenderManager::render_instances(const entt::registry& registry, const Renderer& renderer) {
		auto view = registry.view<Mesh, Material, Position, Renderable, Instances>();

		// todo: accept as arg
		Camera camera({
			glm::vec3(5.0f, 5.0f, 5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			});

		for (auto entity : view) {
			auto& mesh = view.get<Mesh>(entity);
			auto& material = view.get<Material>(entity);
			auto& position = view.get<Position>(entity);
			auto& instances = view.get<Instances>(entity);
			auto renderable = view.get<Renderable>(entity);

			if (renderable) {
				renderer.render_instances(camera, mesh, material, instances);
			}
		}
	}

} // namespace bnp
