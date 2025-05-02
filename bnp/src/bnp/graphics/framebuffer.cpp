#include <bnp/graphics/framebuffer.h>

namespace bnp {

	void Framebuffer::create(int w, int h) {
		// Color texture
		glGenTextures(1, &color_texture_id);
		glBindTexture(GL_TEXTURE_2D, color_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Depth renderbuffer
		glGenRenderbuffers(1, &depth_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

		// Framebuffer
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw std::runtime_error("Framebuffer not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, width, height);
	}

	void Framebuffer::destroy() {
		glDeleteTextures(1, &color_texture_id);
		glDeleteRenderbuffers(1, &depth_rbo);
		glDeleteFramebuffers(1, &fbo);
	}

}
