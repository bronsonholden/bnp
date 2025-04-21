#pragma once

#include <bnp/core/node.hpp>
#include <bnp/graphics/renderer.h>
#include <bnp/factories/material_factory.h>

namespace bnp {

	class RenderManager {
	public:
		RenderManager();
		~RenderManager();

		void render(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_wireframes(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_instances(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_sprites(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_water2d(const entt::registry& registry, const Renderer& renderer, const Camera& camera);

	private:
		// built-in resources
		Mesh sprite_mesh;
		Material wireframe_material;
	};

} // namespace bnp
