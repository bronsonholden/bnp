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
		float speed;
		//bool sprinting;
		//float sprinting_mul;
		bool falling = false;
		bool idle = true;
		bool jumping = false;
		bool walking = false;
		bool crouching = false;
		glm::vec3 impulse = glm::vec3(0.0f);
		glm::vec3 current_velocity = glm::vec3(0.0f);
	};

}
