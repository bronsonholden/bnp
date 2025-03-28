#include <bnp/engine.h>
#include <iostream>
using namespace std;

namespace bnp {
	Engine::Engine() {

	}

	Engine::~Engine() {

	}

	void Engine::run() {
		float accumulator = 0.0f;
		const float fixed_dt = 1.0f / 60.0f;

		time.start();

		while (window.open) {
			window.poll();

			time.new_frame();

			float dt = time.delta_time();

			while (time.needs_fixed_update()) {
				fixed_update();
				time.consume_fixed_step();
			}

			// manager updates

			window.clear();
			window.swap();
		}
	}

	void Engine::update(float dt) {

	}

	void Engine::fixed_update() {

	}
}
