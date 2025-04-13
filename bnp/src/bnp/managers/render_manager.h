#pragma once

#include <bnp/core/node.hpp>
#include <bnp/graphics/renderer.h>

namespace bnp {

	class RenderManager {
	public:
		RenderManager();
		~RenderManager();

		void render(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_instances(const entt::registry& registry, const Renderer& renderer, const Camera& camera);
		void render_sprites(const entt::registry& registry, const Renderer& renderer, const Camera& camera);

	private:
		Mesh sprite_mesh;
	};

} // namespace bnp
