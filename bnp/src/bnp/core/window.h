#pragma once

#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

namespace bnp {
	class Window {
	public:
		bool open;

		Window();
		~Window();
		int poll(SDL_Event& event);
		void clear();
		void swap();

		SDL_Window* get_sdl_window();
		SDL_GLContext* get_gl_context();

		int get_width() const;
		int get_height() const;

	private:
		SDL_Window* w;
		SDL_GLContext glContext;

		void handle_window_event(SDL_Event& event);
	};
}
