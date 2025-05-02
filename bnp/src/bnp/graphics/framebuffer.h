#pragma once

#include <gl/glew.h>

namespace bnp {

	class Framebuffer {
	public:
		Framebuffer() = default;

		void create(int w, int h);
		void bind();
		void destroy();

	private:
		bool init;
		GLuint fbo;
		GLuint color_texture_id;
		GLuint depth_rbo;
	};

}
