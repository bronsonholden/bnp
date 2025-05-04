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
		// primarily used for 2D rendering. lower values render first
		uint32_t render_order = 0;
	};

	struct Vertex {
		float position[3];
		float normal[3];
		float tex_coords[2];
	};

	struct SpriteFrame {
		float duration = 0.0f;
		glm::vec2 uv0;
		glm::vec2 uv1;
		glm::ivec2 pixel_offset = {};
		glm::ivec2 size = {};
		glm::ivec4 coords;
	};

	struct SpriteLayer {
		std::string name;
		uint32_t layer_index;
		bool visible;
		std::vector<SpriteFrame> frames;
	};

	struct SpriteAnimation {
		// which frames are included in the animation
		std::vector<uint32_t> framelist;

		// currently: if 0, loop, otherwise play once. todo
		uint32_t repeat = 0;
	};

	struct Sprite {
		int z = 0;
		SpriteFrame default_frame;
		uint32_t spritesheet_width;
		uint32_t spritesheet_height;
		glm::ivec2 mirror = { 1, 1 };
		std::unordered_map<std::string, SpriteAnimation> animations;
		std::unordered_map<std::string, std::unordered_map<uint32_t, glm::ivec4>> slices;
		std::vector<SpriteLayer> layers;
		uint32_t frame_count;
	};

	struct SpriteAnimator {
		std::string current_animation;

		// which framelist index is currently being displayed
		size_t current_framelist_index = 0;

		float time = 0.0f;
		bool playing = true;

		// ignored in favor of SpriteAnimation::repeat for now
		bool loop = true;
		bool done = false;
	};

	// maybe todo: add `managed` prop. If unmanaged, cleanup automatically happens
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

	// maybe todo: add `managed` prop. If unmanaged, cleanup automatically happens
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

	struct BezierSprite {
		glm::vec2 p0;
		glm::vec2 p1;
		glm::vec2 p2;
		int steps = 32;

		GLuint vao = 0;
		GLuint vbo = 0;
		GLuint vertex_count = 0;

		glm::vec2 point(float t) {
			float u = 1.0f - t;
			return u * u * p0 + 2 * u * t * p1 + t * t * p2;
		}

		std::vector<glm::vec2> generate_points() {
			std::vector<glm::vec2> points;

			for (int i = 0; i <= steps; ++i) {
				float t = i / static_cast<float>(steps);
				glm::vec2 pt = point(t);
				pt = glm::floor(pt * 48.0f) / 48.0f; // Pixel-snapping!
				points.push_back(pt);
			}

			return points;
		}

		std::vector<glm::vec3> generate_thick_line_strip(float thickness) {
			std::vector<glm::vec2> points = generate_points();
			std::vector<glm::vec3> vertices;

			if (points.size() < 2) return vertices;

			const float half_thickness = thickness / 2.0f;

			for (size_t i = 0; i < points.size() - 1; ++i) {
				glm::vec2 p0 = points[i];
				glm::vec2 p1 = points[i + 1];

				// Create a horizontal quad: offset in Y only
				glm::vec2 a2 = glm::vec2(p0.x, p0.y - half_thickness);
				glm::vec2 b2 = glm::vec2(p0.x, p0.y + half_thickness);
				glm::vec2 c2 = glm::vec2(p1.x, p1.y + half_thickness);
				glm::vec2 d2 = glm::vec2(p1.x, p1.y - half_thickness);

				// Convert to vec3 with z = 0
				glm::vec3 a = glm::vec3(a2, 0.0f);
				glm::vec3 b = glm::vec3(b2, 0.0f);
				glm::vec3 c = glm::vec3(c2, 0.0f);
				glm::vec3 d = glm::vec3(d2, 0.0f);

				// Two triangles
				vertices.push_back(a);
				vertices.push_back(b);
				vertices.push_back(c);

				vertices.push_back(a);
				vertices.push_back(c);
				vertices.push_back(d);
			}

			return vertices;
		}

		void update_buffer_data() {
			if (vao) glDeleteVertexArrays(1, &vao);
			if (vbo) glDeleteBuffers(1, &vbo);

			std::vector<glm::vec3> quad_vertices = generate_thick_line_strip(0.1f);

			vertex_count = quad_vertices.size();

			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);

			for (auto& vertex : quad_vertices) {
				cout << vertex.x << ", " << vertex.y << endl;
				break;
			}

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, quad_vertices.size() * sizeof(glm::vec3), quad_vertices.data(), GL_STATIC_DRAW);

			// Position attribute at location 0
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

			glBindVertexArray(0);
		}

	};

}
