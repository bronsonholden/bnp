#pragma once

#include <bnp/components/transform.h>

#include <glm/glm.hpp>
#include <gl/glew.h>

#include <vector>
#include <unordered_map>
#include <iostream>
using namespace std;

namespace bnp {

	enum ShaderType {
		VertexShader = 1,
		FragmentShader
	};

	struct Renderable {
		bool value;
	};

	struct Vertex {
		float position[3];
		float normal[3];
		float tex_coords[2];
	};

	struct SpriteFrame {
		uint32_t frame_index;
		float duration = 0.0f;
		glm::vec2 uv0;
		glm::vec2 uv1;
		glm::ivec2 pixel_offset = {};
		glm::ivec2 size = {};
	};

	struct SpriteAnimation {
		std::string name;
		std::vector<SpriteFrame> frames;
	};

	struct Sprite {
		int layer = 0;
		SpriteFrame default_frame;
		uint32_t spritesheet_width;
		uint32_t spritesheet_height;
		uint32_t frame_width;
		uint32_t frame_height;
		glm::ivec2 mirror = { 1, 1 };
		std::unordered_map<std::string, SpriteAnimation> animations;
	};

	struct SpriteAnimator {
		std::string current_animation;
		size_t current_frame_index = 0;
		float time = 0.0f;
		bool playing = true;
		bool loop = true;
		bool done = false;
	};

	struct Mesh {
		std::string resource_id;

		// vertex array object
		GLuint va_id = 0;
		// vertex buffer object
		GLuint vb_id = 0;
		// element buffer object
		GLuint eb_id = 0;

		size_t vertex_count = 0;

		void cleanup() {
			if (va_id) glDeleteVertexArrays(1, &va_id);
			if (eb_id) glDeleteBuffers(1, &eb_id);
			if (vb_id) glDeleteBuffers(1, &vb_id);

			va_id = vb_id = eb_id = 0;
		}
	};

	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

	struct Texture {
		std::string resource_id;
		GLuint texture_id = 0;
		uint8_t channels = 0;

		Texture() : texture_id(0) { }

		void cleanup() {
			if (texture_id) {
				glDeleteTextures(1, &texture_id);
			}
		}
	};

	struct ShaderData {
		ShaderType shader_type;
		std::string source;
	};

	struct Material {
		std::string vertex_shader_resource_id;
		std::string fragment_shader_resource_id;
		GLuint shader_id = 0;

		Material() : shader_id(0) { }

		//Material(const Material& other)
		//	: vertex_shader_resource_id(other.vertex_shader_resource_id),
		//	fragment_shader_resource_id(other.fragment_shader_resource_id),
		//	shader_id(other.shader_id)
		//{
		//}

		//Material(Material&& other) noexcept
		//	: vertex_shader_resource_id(std::move(other.vertex_shader_resource_id)),
		//	fragment_shader_resource_id(std::move(other.fragment_shader_resource_id))
		//{
		//}
	};

	struct Camera {
		glm::vec3 position = glm::vec3(5.0f);
		glm::vec3 target = glm::vec3(0.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 perspective = glm::perspective(glm::radians(45.0f), 1280.0f / 920.0f, 0.1f, 100.0f);
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
