#include <bnp/graphics/renderer.h>

#include <GL/glew.h>
#include <string>
#include <iostream>
using namespace std;

namespace bnp {

	Renderer::Renderer()
		: front_fb(),
		obstacle_fb()
	{
	}

	Renderer::~Renderer() {
		shutdown();
	}

	void Renderer::initialize(int width, int height) {
		glewInit();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);

		resize(width, height);
	}

	void Renderer::shutdown() {

	}

	void Renderer::resize(int width, int height) {
		//float aspect = static_cast<float>(width) / static_cast<float>(height);

		// downscale but keep aspect ratio
		//int downscaled_height = 240;
		//int downscaled_width = static_cast<int>(std::floor(height * aspect));

		front_fb.create(width, height);
		obstacle_fb.create(width, height);
	}

	void Renderer::render_line(const Camera& camera, const Mesh& mesh, const Material& material, const glm::mat4& world_transform) const {
		glUseProgram(material.shader_id);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);

		glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
		glm::mat4 projection = camera.perspective;

		// Set the transform matrices
		GLuint model_loc = glGetUniformLocation(material.shader_id, "model");
		GLuint view_loc = glGetUniformLocation(material.shader_id, "view");
		GLuint proj_loc = glGetUniformLocation(material.shader_id, "projection");
		GLuint color_loc = glGetUniformLocation(material.shader_id, "color");

		glm::vec4 color(1, 0, 0, 1);

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, &world_transform[0][0]);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);
		glUniform4fv(color_loc, 1, &color[0]);

		glBindVertexArray(mesh.va_id);

		if (mesh.eb_id) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eb_id);
		}

		glDrawArrays(GL_LINES, 0, 2);
	}

	void Renderer::render_sprite(const Camera& camera, const Sprite& sprite, const SpriteFrame& sprite_frame, const Mesh& mesh, const Material& material, const Texture& texture, const glm::mat4& transform) const {
		glUseProgram(material.shader_id);

		if (texture.channels < 4) {
			glDisable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);

		glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
		glm::mat4 projection = camera.perspective;

		// Set the transform matrices
		GLuint model_loc = glGetUniformLocation(material.shader_id, "model");
		GLuint view_loc = glGetUniformLocation(material.shader_id, "view");
		GLuint proj_loc = glGetUniformLocation(material.shader_id, "projection");
		GLuint uv0_loc = glGetUniformLocation(material.shader_id, "uv0");
		GLuint uv1_loc = glGetUniformLocation(material.shader_id, "uv1");
		GLuint spritesheet_size_loc = glGetUniformLocation(material.shader_id, "spritesheet_size");

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, &transform[0][0]);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);
		glUniform2f(uv0_loc, sprite_frame.uv0.x, sprite_frame.uv0.y);
		glUniform2f(uv1_loc, sprite_frame.uv1.x, sprite_frame.uv1.y);
		glUniform2f(spritesheet_size_loc, sprite.spritesheet_width, sprite.spritesheet_height);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.texture_id);
		glUniform1i(glGetUniformLocation(material.shader_id, "sprite_texture"), 0);

		glBindVertexArray(mesh.va_id);

		if (mesh.eb_id) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eb_id);
		}

		glDrawElements(GL_TRIANGLES, mesh.vertex_count, GL_UNSIGNED_INT, 0);

		if (texture.channels < 4) {
			glEnable(GL_BLEND);
		}
	}

	void Renderer::render_fullscreen_quad(const Mesh& mesh, const Material& material, const Framebuffer& framebuffer) const {
		glUseProgram(material.shader_id);

		glDisable(GL_DEPTH_TEST);

		// Set sampler uniform to texture unit 0
		GLint location = glGetUniformLocation(material.shader_id, "screenTexture");
		glUniform1i(location, 0);

		// Vertex data: fullscreen quad in clip space
		float quad_vertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,   0.0f, 1.0f,
			-1.0f, -1.0f,   0.0f, 0.0f,
			 1.0f, -1.0f,   1.0f, 0.0f,

			-1.0f,  1.0f,   0.0f, 1.0f,
			 1.0f, -1.0f,   1.0f, 0.0f,
			 1.0f,  1.0f,   1.0f, 1.0f
		};

		GLuint vao, vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // aPos
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1); // aTexCoord
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		// Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffer.color_texture_id);

		// Draw the quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Cleanup (optional since we're discarding VAO/VBO anyway)
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);

		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::render_wireframe(const Camera& camera, const Mesh& mesh, const Material& material, const glm::mat4& world_transform, const glm::vec4& color, bool fill) const {
		glUseProgram(material.shader_id);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);

		glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
		glm::mat4 projection = camera.perspective;

		// Set the transform matrices
		GLuint model_loc = glGetUniformLocation(material.shader_id, "model");
		GLuint view_loc = glGetUniformLocation(material.shader_id, "view");
		GLuint proj_loc = glGetUniformLocation(material.shader_id, "projection");
		GLuint color_loc = glGetUniformLocation(material.shader_id, "color");

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, &world_transform[0][0]);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);
		glUniform4fv(color_loc, 1, &color[0]);

		glBindVertexArray(mesh.va_id);

		if (mesh.eb_id) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eb_id);
		}

		if (!fill) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, mesh.vertex_count, GL_UNSIGNED_INT, 0);
		if (!fill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void Renderer::render(const Camera& camera, const Mesh& mesh, const Material& material, const Texture& texture, const glm::mat4& transform) const {
		glUseProgram(material.shader_id);

		if (texture.channels == 4) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);

		glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
		glm::mat4 projection = camera.perspective;

		// Set the transform matrices
		GLuint model_loc = glGetUniformLocation(material.shader_id, "model");
		GLuint view_loc = glGetUniformLocation(material.shader_id, "view");
		GLuint proj_loc = glGetUniformLocation(material.shader_id, "projection");

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, &transform[0][0]);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.texture_id);
		glUniform1i(glGetUniformLocation(material.shader_id, "sprite_texture"), 0);

		glBindVertexArray(mesh.va_id);

		if (mesh.eb_id) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eb_id);
		}

		glDrawElements(GL_TRIANGLES, mesh.vertex_count, GL_UNSIGNED_INT, 0);

		if (texture.channels == 4) {
			glDisable(GL_BLEND);
		}
	}

	void Renderer::render_instances(const Camera& camera, const Mesh& mesh, const Material& material, const Texture& texture, const Instances& instances) const {
		glUseProgram(material.shader_id);

		if (texture.channels == 4) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);

		// Camera view and projection matrices
		glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
		glm::mat4 projection = camera.perspective;

		// Set the uniform locations for matrices
		GLuint view_loc = glGetUniformLocation(material.shader_id, "view");
		GLuint proj_loc = glGetUniformLocation(material.shader_id, "projection");
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);

		// Set color (for now, hardcoded)
		GLuint color_loc = glGetUniformLocation(material.shader_id, "color");
		glUniform3f(color_loc, 0.5f, 0.8f, 0.2f);

		glBindBuffer(GL_ARRAY_BUFFER, instances.vb_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eb_id);

		// Bind VAO and set up instanced attribute pointers
		glBindVertexArray(mesh.va_id);

		// Model matrix attribute (4 vec4s per mat4)
		GLuint model_loc = glGetAttribLocation(material.shader_id, "model");
		for (int i = 0; i < 4; i++) {
			glEnableVertexAttribArray(model_loc + i);
			glVertexAttribPointer(model_loc + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(model_loc + i, 1);
		}

		// Draw instanced
		glDrawElementsInstanced(GL_TRIANGLES, mesh.vertex_count, GL_UNSIGNED_INT, 0, instances.transforms.size());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if (texture.channels == 4) {
			glDisable(GL_BLEND);
		}
	}

} // namespace bnp
