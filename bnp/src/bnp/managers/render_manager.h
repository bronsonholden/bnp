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
		void render_fullscreen_quad(const Renderer& renderer, const Framebuffer& framebuffer);
		void render_flow_field_2ds(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_wireframes(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_instances(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_sprites(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_water2d(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_physics_body_2ds(const entt::registry& registry, const Renderer& renderer, const Camera& camera);

		bool render_flow_field_2d_reverse;

	private:
		// built-in resources
		Mesh sprite_mesh;
		Mesh line_mesh;
		Material wireframe_material;
		Material quad_material;
		Material obstacle_material;
	};

} // namespace bnp
