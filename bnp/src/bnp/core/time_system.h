#pragma once

#include <chrono>

namespace bnp {
class TimeSystem {
public:
	using clock = std::chrono::high_resolution_clock;
	using time_point = clock::time_point;

	void start();               // Call at the beginning of the engine
	void new_frame();          // Call once per frame at the top of the loop

	// Accessors
	float delta_time() const;          // Time between last frame and this one
	float fixed_delta_time() const;    // Fixed timestep (defaults to 1/60)
	float total_time() const;          // Total runtime (seconds)
	float accumulator() const;         // Time left for fixed updates
	int frame_count() const;           // How many frames since start

	// Fixed timestep control
	void set_fixed_delta_time(float dt);
	bool needs_fixed_update() const;
	void consume_fixed_step();          // Reduce accumulator after fixed update

private:
	time_point previous_time_;
	time_point start_time_;

	float delta_time_ = 0.0f;
	float fixed_delta_time_ = 1.0f / 120.0f;
	float accumulator_ = 0.0f;
	float total_time_ = 0.0f;

	int frame_count_ = 0;
};
}
