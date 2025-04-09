#include <bnp/graphics/renderer.h>

#include <GL/glew.h>
#include <string>
#include <iostream>
using namespace std;

namespace bnp {

	Renderer::Renderer() {
	}

	Renderer::~Renderer() {
		shutdown();
	}

	void Renderer::initialize() {
		glewInit();
		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::render(const Camera& camera, const Mesh& mesh, const Material& material, const glm::mat4& transform) const {
		glUseProgram(material.shader_id);

		glm::mat4 view = glm::lookAt(
			camera.position,
			camera.target,
			camera.up
		);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

		// Set the transform matrices
		GLuint model_loc = glGetUniformLocation(material.shader_id, "model");
		GLuint view_loc = glGetUniformLocation(material.shader_id, "view");
		GLuint proj_loc = glGetUniformLocation(material.shader_id, "projection");

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, &transform[0][0]);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);

		// Set color
		GLuint color_loc = glGetUniformLocation(material.shader_id, "color");
		glUniform3f(color_loc, 0.5f, 0.8f, 0.2f);

		glBindVertexArray(mesh.va_id);
		glDrawElements(GL_TRIANGLES, mesh.vertex_count, GL_UNSIGNED_INT, 0);
	}

	void Renderer::render_instances(const Camera& camera, const Mesh& mesh, const Material& material, const Instances& instances) const {
		glUseProgram(material.shader_id);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);

		// Camera view and projection matrices
		glm::mat4 view = glm::lookAt(camera.position, camera.target, camera.up);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

		// Set the uniform locations for matrices
		GLuint view_loc = glGetUniformLocation(material.shader_id, "view");
		GLuint proj_loc = glGetUniformLocation(material.shader_id, "projection");
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);

		// Set color (for now, hardcoded)
		GLuint color_loc = glGetUniformLocation(material.shader_id, "color");
		glUniform3f(color_loc, 0.5f, 0.8f, 0.2f);

		glBindBuffer(GL_ARRAY_BUFFER, instances.vb_id);

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
	}


	void Renderer::shutdown() {
	}

} // namespace bnp
