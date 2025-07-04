#include <bnp/core/window.h>

namespace bnp {
Window::Window() {
	this->w = NULL;
	this->open = true;
	this->glContext = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw SDL_GetError();
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	this->w = SDL_CreateWindow(
		"SDL window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1920,
		1080,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	if (!this->w) {
		throw SDL_GetError();
	}

	this->glContext = SDL_GL_CreateContext(this->w);
	if (!this->glContext) {
		throw SDL_GetError();
	}

	int glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		throw glewGetErrorString(glewError);
	}
}

Window::~Window() {
	if (this->glContext) {
		SDL_GL_DeleteContext(this->glContext);
	}

	if (this->w) {
		SDL_DestroyWindow(this->w);
	}

	SDL_Quit();
}

int Window::poll(SDL_Event& event) {
	return SDL_PollEvent(&event);
}

void Window::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::swap() {
	SDL_GL_SwapWindow(this->w);
}

int Window::get_width() const {
	int width;
	SDL_GetWindowSize(w, &width, nullptr);
	return width;
}

int Window::get_height() const {
	int height;
	SDL_GetWindowSize(w, nullptr, &height);
	return height;
}

void Window::get_mouse_position(int* x, int* y) const {
	SDL_GetMouseState(x, y);
}

SDL_Window* Window::get_sdl_window() {
	return w;
}

SDL_GLContext* Window::get_gl_context() {
	return &glContext;
}
}
