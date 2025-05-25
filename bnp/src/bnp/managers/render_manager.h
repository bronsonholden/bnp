#pragma once

#include <bnp/core/node.hpp>
#include <bnp/graphics/renderer.h>

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
		void render_planet_2ds(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_bezier_sprites(const entt::registry& registry, const Renderer& renderer, const Camera& camera);

		bool debug_render_wireframes;

		// quad mesh for rendering sprites
		Mesh sprite_mesh;
		// mesh for rendering a single line
		Mesh line_mesh;
		// material for rendering debug wireframes
		Material wireframe_material;
		// material for rendering fullscreen quad
		Material quad_material;
		// material for rendering 2D physics bodies
		Material physics_body_2d_material;
	};

} // namespace bnp
