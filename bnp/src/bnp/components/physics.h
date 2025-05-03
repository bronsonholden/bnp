#pragma once

#include <bnp/components/graphics.h>

#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace bnp {

	struct PhysicsBody2D {
		b2Body* body = nullptr;
		Mesh mesh;
	};

	struct Motility {
		float speed = 1.0f;
		float jump_height = 2.5f;
		float current_air_control = 1.0f;
		float air_control_ramp = 0.985f;
		float fall_time = 0.0f;
		// how quickly directions can change when flying
		float flying_response = 0.02f;
		//bool sprinting;
		//float sprinting_mul;
		bool air_control = false;
		bool falling = false;
		bool idle = true;
		bool start_jump = false;
		bool jumping = false;
		bool walking = false;
		bool crouching = false;
		bool flying = false;
		glm::vec3 impulse = glm::vec3(0.0f);
		glm::vec3 current_velocity = glm::vec3(0.0f);
	};

	struct Water2D {
		int columns = 0;
		float column_width = 0.05f;
		std::vector<float> height;
		std::vector<float> velocity;
		std::vector<float> base_height;
	};

}
