#pragma once

#include <gl/glew.h>

namespace bnp {

	class Framebuffer {
	public:
		Framebuffer();
		~Framebuffer();

		void create(int w, int h);
		void bind();
		void unbind();
		void destroy();

	private:
		bool init;
		int width;
		int height;
		GLuint fbo;
		GLuint color_texture_id;
		GLuint depth_rbo;
	};

}
