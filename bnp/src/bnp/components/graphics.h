#pragma once

#include <bnp/components/transform.h>

#include <glm/glm.hpp>
#include <gl/glew.h>
#include <vector>

#include <iostream>
using namespace std;

namespace bnp {

	struct Renderable {
		bool value;
	};

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
		std::vector<Transform> transforms;
		std::vector<glm::mat4> world_transforms;
		GLuint vb_id;
		bool dirty;

		Instances() : vb_id(0), dirty(true) {
		}

		~Instances() {
		}

		void cleanup() {
			if (vb_id) {
				glDeleteBuffers(1, &vb_id);
			}
		}

		void update_transforms() {
			if (!dirty) return;

			if (!vb_id) {
				glGenBuffers(1, &vb_id);
			}

			const size_t count = transforms.size();

			if (count > 0) {
				world_transforms.resize(count);
			}

			for (int i = 0; i < transforms.size(); ++i) {
				world_transforms[i] = transforms[i].world_transform;
			}

			// Update instance buffer data
			glBindBuffer(GL_ARRAY_BUFFER, vb_id);
			glBufferData(GL_ARRAY_BUFFER, world_transforms.size() * sizeof(glm::mat4), world_transforms.data(), GL_DYNAMIC_DRAW);

			dirty = false;
		}
	};

}
