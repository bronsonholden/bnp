#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/quaternion.hpp>

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

		void update_world_transform(const glm::mat4& parent_world_transform) {
			glm::mat4 local_transform =
				glm::translate(glm::mat4(1.0f), position) *
				glm::toMat4(rotation) *
				glm::scale(glm::mat4(1.0f), scale);

			world_transform = parent_world_transform * local_transform;

			dirty = false;
		}
	};
}
