#pragma once

#include <bnp/components/graphics.h>

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bnp {

	class Renderer {
	public:
		Renderer();
		~Renderer();

		void initialize();
		void render(const Camera& camera, const Mesh& mesh, const Material& material, const glm::mat4& transform) const;
		void render_instances(const Camera& camera, const Mesh& mesh, const Material& material, const Instances& instances) const;
		void shutdown();
	};

} // namespace bnp
