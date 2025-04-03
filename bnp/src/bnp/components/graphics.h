#pragma once

#include <bnp/components/transform.h>

#include <glm/glm.hpp>
#include <gl/glew.h>
#include <vector>

namespace bnp {

	typedef bool Renderable;

	struct Vertex {
		float position[3];
		float normal[3];
		float tex_coords[2];
	};

	struct Mesh {
		// vertex array object
		GLuint va_id;
		// vertex buffer object
		GLuint vb_id;
		// element buffer object
		GLuint eb_id;

		size_t vertex_count;
	};

	struct Texture {
		GLuint texture_id;
	};

	struct Material {
		GLuint shader_id;
	};

	struct Camera {
		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 up;
	};

	// use for static entities where each instance has no
	// indivdual state other than their transform
	struct Instances {
		std::vector<Position> positions;
		std::vector<Rotation> rotations;
		std::vector<Scale> scales;
		std::vector<glm::mat4> transforms;
		GLuint vb_id;

		Instances() {
			glGenBuffers(1, &vb_id);

		}

		~Instances() {
			glDeleteBuffers(1, &vb_id);
		}

		void update_transforms() {
			transforms.resize(positions.size());

			for (int i = 0; i < positions.size(); ++i) {
				glm::mat4 transform = glm::mat4(1.0f);

				transform = glm::translate(transform, positions.at(i).value);
				transform *= glm::mat4_cast(rotations.at(i).value);
				transform = glm::scale(transform, scales.at(i).value);

				transforms[i] = transform;
			}
		}
	};

}
