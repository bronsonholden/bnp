#pragma once

#include <bnp/core/node.hpp>
#include <bnp/graphics/renderer.h>

namespace bnp {

	class RenderManager {
	public:
		RenderManager() = default;

		void render(const entt::registry& registry, const Renderer& renderer);
		void render_instances(const entt::registry& registry, const Renderer& renderer);
	};

} // namespace bnp
