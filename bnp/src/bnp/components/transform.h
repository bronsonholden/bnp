#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
using namespace std;

namespace bnp {

	struct Transform {
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		glm::mat4 world_transform;
		bool dirty;

		Transform(glm::vec3 _position = glm::vec3(0.0f), glm::quat _rotation = glm::quat(), glm::vec3 _scale = glm::vec3(1.0f))
			: position(_position),
			rotation(_rotation),
			scale(_scale),
			world_transform(glm::mat4(1.0f)),
			dirty(true)
		{
		}

		void update_world_transform() {
			world_transform = glm::translate(world_transform, position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
			dirty = false;
		}
	};
}
