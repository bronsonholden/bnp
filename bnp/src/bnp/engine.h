#pragma once

#include <bnp/core/window.h>
#include <bnp/core/time_system.h>

#include <entt/entt.hpp>

namespace bnp {
	class Engine {
	public:
		Engine();
		~Engine();

		void run();

	protected:
		entt::registry registry;
		Window window;
		TimeSystem time;

		void update(float dt);
		void fixed_update();
	};
}
