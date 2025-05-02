#pragma once

#include <bnp/graphics/framebuffer.h>
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

		void initialize(int width, int height);
		void shutdown();

		void resize(int width, int height);

		void render(const Camera& camera, const Mesh& mesh, const Material& material, const Texture& texture, const glm::mat4& transform) const;
		void render_line(const Camera& camera, const Mesh& mesh, const Material& material, const glm::mat4& world_transform) const;
		void render_wireframe(const Camera& camera, const Mesh& mesh, const Material& material, const glm::mat4& world_transform, const glm::vec4& color, bool fill = false) const;
		void render_sprite(const Camera& camera, const Sprite& sprite, const SpriteFrame& sprite_frame, const Mesh& mesh, const Material& material, const Texture& texture, const glm::mat4& transform) const;
		void render_instances(const Camera& camera, const Mesh& mesh, const Material& material, const Texture& texture, const Instances& instances) const;

	private:
		Framebuffer front_fb;
	};

} // namespace bnp
