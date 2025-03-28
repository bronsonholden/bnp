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
		int poll();
		void clear();
		void swap();

	private:
		SDL_Window* w;
		SDL_GLContext glContext;
	};
}
