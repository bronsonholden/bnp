#include <bnp/graphics/framebuffer.h>

#include <stdexcept>

namespace bnp {

	Framebuffer::Framebuffer() {
		width = 0;
		height = 0;
		color_texture_id = 0;
		depth_rbo = 0;
		fbo = 0;
		init = false;
	}

	Framebuffer::~Framebuffer() {
		destroy();
	}

	void Framebuffer::create(int w, int h, int channels) {
		if (init) destroy();

		GLenum internal_format = GL_RGB;
		GLenum format = GL_RGB;

		switch (channels) {
		case 1:
			internal_format = GL_R8;
			format = GL_RED;
			break;
		case 3:
			internal_format = GL_RGB8;
			format = GL_RGB;
			break;
		case 4:
			internal_format = GL_RGBA8;
			format = GL_RGBA;
			break;
		default:
			throw std::runtime_error("Unsupported framebuffer channel count");
		}

		// Color texture
		glGenTextures(1, &color_texture_id);
		glBindTexture(GL_TEXTURE_2D, color_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Depth renderbuffer
		glGenRenderbuffers(1, &depth_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

		// Framebuffer
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw std::runtime_error("Framebuffer not complete!");
		}

		init = true;
		width = w;
		height = h;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::bind() {
		if (!init) return;

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		// todo: maybe keep this?
		glViewport(0, 0, width, height);
	}

	void Framebuffer::clear() {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Framebuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::destroy() {
		if (!init) return;

		glDeleteTextures(1, &color_texture_id);
		glDeleteRenderbuffers(1, &depth_rbo);
		glDeleteFramebuffers(1, &fbo);
	}

}
