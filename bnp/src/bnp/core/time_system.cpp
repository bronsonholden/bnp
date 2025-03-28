#include <bnp/core/time_system.h>

namespace bnp {
	void TimeSystem::start() {
		start_time_ = clock::now();
		previous_time_ = start_time_;
		delta_time_ = 0.0f;
		accumulator_ = 0.0f;
		total_time_ = 0.0f;
		frame_count_ = 0;
	}

	void TimeSystem::new_frame() {
		time_point now = clock::now();
		delta_time_ = std::chrono::duration<float>(now - previous_time_).count();
		previous_time_ = now;

		accumulator_ += delta_time_;
		total_time_ = std::chrono::duration<float>(now - start_time_).count();
		frame_count_++;
	}

	float TimeSystem::delta_time() const {
		return delta_time_;
	}

	float TimeSystem::fixed_delta_time() const {
		return fixed_delta_time_;
	}

	float TimeSystem::total_time() const {
		return total_time_;
	}

	float TimeSystem::accumulator() const {
		return accumulator_;
	}

	int TimeSystem::frame_count() const {
		return frame_count_;
	}

	void TimeSystem::set_fixed_delta_time(float dt) {
		fixed_delta_time_ = dt;
	}

	bool TimeSystem::needs_fixed_update() const {
		return accumulator_ >= fixed_delta_time_;
	}

	void TimeSystem::consume_fixed_step() {
		accumulator_ -= fixed_delta_time_;
	}
}
