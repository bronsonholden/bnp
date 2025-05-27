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
		float aspect = static_cast<float>(width) / static_cast<float>(height);
		// downscale but keep aspect ratio
		int downscaled_height = height / 5;
		int downscaled_width = static_cast<int>(std::floor(height * aspect));
		upscale_fb.create(480, 360, 4);

		front_fb.create(width, height);
		obstacle_fb.create(width, height);
	}

	void Renderer::render_line(const Camera& camera, const Mesh& mesh, const Material& material, const glm::mat4& world_transform) const {
		if (!material.shader_id) return;

		glUseProgram(material.shader_id);

		glm::mat4 view = camera.view;
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
		if (!material.shader_id) return;

		glUseProgram(material.shader_id);

		if (texture.channels < 4) {
			glDisable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glm::mat4 view = camera.view;
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

	void Renderer::render_planet_2d(const Camera& camera, const Planet2D& planet_2d, const Mesh& mesh, const Material& material, const glm::mat4& transform) const {
		if (!material.shader_id) return;

		glUseProgram(material.shader_id);

		glm::mat4 view = camera.view;
		glm::mat4 projection = camera.perspective;

		// Set the transform matrices
		GLuint model_loc = glGetUniformLocation(material.shader_id, "model");
		GLuint view_loc = glGetUniformLocation(material.shader_id, "view");
		GLuint proj_loc = glGetUniformLocation(material.shader_id, "projection");
		GLuint time_loc = glGetUniformLocation(material.shader_id, "time");
		GLuint rotation_speed_loc = glGetUniformLocation(material.shader_id, "rotation_speed");
		GLuint noise_radius_loc = glGetUniformLocation(material.shader_id, "noise_radius");
		GLuint noise_seed_loc = glGetUniformLocation(material.shader_id, "noise_seed");
		GLuint axis_loc = glGetUniformLocation(material.shader_id, "axis");
		GLuint sun_direction_loc = glGetUniformLocation(material.shader_id, "sun_direction");
		GLuint water_depth_loc = glGetUniformLocation(material.shader_id, "water_depth");
		GLuint coast_depth_loc = glGetUniformLocation(material.shader_id, "coast_depth");
		GLuint mainland_depth_loc = glGetUniformLocation(material.shader_id, "mainland_depth");
		GLuint water_color_loc = glGetUniformLocation(material.shader_id, "water_color");
		GLuint coast_color_loc = glGetUniformLocation(material.shader_id, "coast_color");
		GLuint mainland_color_loc = glGetUniformLocation(material.shader_id, "mainland_color");
		GLuint mountain_color_loc = glGetUniformLocation(material.shader_id, "mountain_color");
		GLuint cloud_depth_loc = glGetUniformLocation(material.shader_id, "cloud_depth");
		GLuint cloud_radius_loc = glGetUniformLocation(material.shader_id, "cloud_radius");
		GLuint cloud_radius_equator_exp_loc = glGetUniformLocation(material.shader_id, "cloud_radius_equator_exp");
		GLuint cloud_banding_equator_exp_loc = glGetUniformLocation(material.shader_id, "cloud_banding_equator_exp");
		GLuint crater_step_loc = glGetUniformLocation(material.shader_id, "crater_step");
		GLuint num_craters_loc = glGetUniformLocation(material.shader_id, "num_craters");

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, &transform[0][0]);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);
		glUniform1f(time_loc, planet_2d.time);
		glUniform1f(rotation_speed_loc, planet_2d.rotation_speed);
		glUniform1f(noise_radius_loc, planet_2d.noise_radius);
		glUniform1f(noise_seed_loc, planet_2d.noise_seed);
		glUniform3f(axis_loc, planet_2d.axis.x, planet_2d.axis.y, planet_2d.axis.z);
		glUniform3f(sun_direction_loc, planet_2d.sun_direction.x, planet_2d.sun_direction.y, planet_2d.sun_direction.z);
		glUniform1f(water_depth_loc, planet_2d.water_depth);
		glUniform1f(coast_depth_loc, planet_2d.coast_depth);
		glUniform1f(mainland_depth_loc, planet_2d.mainland_depth);
		glUniform3f(water_color_loc, planet_2d.water_color.x, planet_2d.water_color.y, planet_2d.water_color.z);
		glUniform3f(coast_color_loc, planet_2d.coast_color.x, planet_2d.coast_color.y, planet_2d.coast_color.z);
		glUniform3f(mainland_color_loc, planet_2d.mainland_color.x, planet_2d.mainland_color.y, planet_2d.mainland_color.z);
		glUniform3f(mountain_color_loc, planet_2d.mountain_color.x, planet_2d.mountain_color.y, planet_2d.mountain_color.z);
		glUniform1f(cloud_depth_loc, planet_2d.cloud_depth);
		glUniform1f(cloud_radius_loc, planet_2d.cloud_radius);
		glUniform1f(cloud_radius_equator_exp_loc, planet_2d.cloud_radius_equator_exp);
		glUniform1f(cloud_banding_equator_exp_loc, planet_2d.cloud_banding_equator_exp);
		glUniform1f(crater_step_loc, planet_2d.crater_step);
		glUniform1i(num_craters_loc, planet_2d.num_craters);

		glBindVertexArray(mesh.va_id);

		if (mesh.eb_id) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eb_id);
		}

		glDrawElements(GL_TRIANGLES, mesh.vertex_count, GL_UNSIGNED_INT, 0);
	}

	void Renderer::render_fullscreen_quad(const Mesh& mesh, const Material& material, const Framebuffer& framebuffer) const {
		if (!material.shader_id) return;

		glUseProgram(material.shader_id);

		glDisable(GL_DEPTH_TEST);

		// Set sampler uniform to texture unit 0
		GLint location = glGetUniformLocation(material.shader_id, "screenTexture");
		glUniform1i(location, 0);

		glBindVertexArray(mesh.va_id);

		if (mesh.eb_id) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eb_id);
		}

		// Bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffer.color_texture_id);

		// Draw the quad
		glDrawElements(GL_TRIANGLES, mesh.vertex_count, GL_UNSIGNED_INT, 0);

		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::render_bezier_sprite(const Camera& camera, const BezierSprite& sprite, const Material& material, const glm::mat4& world_transform) const {
		if (!material.shader_id) return;

		glUseProgram(material.shader_id);

		glm::mat4 view = camera.view;
		glm::mat4 projection = camera.perspective;

		// Set the transform matrices
		GLuint model_loc = glGetUniformLocation(material.shader_id, "model");
		GLuint view_loc = glGetUniformLocation(material.shader_id, "view");
		GLuint proj_loc = glGetUniformLocation(material.shader_id, "projection");

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, &world_transform[0][0]);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection[0][0]);

		glBindVertexArray(sprite.vao);
		// Draw the quads
		glDrawArrays(GL_TRIANGLES, 0, sprite.vertex_count);
		glEnable(GL_DEPTH_TEST);
	}

	void Renderer::render_wireframe(const Camera& camera, const Mesh& mesh, const Material& material, const glm::mat4& world_transform, const glm::vec4& color, bool fill) const {
		if (!material.shader_id) return;

		glUseProgram(material.shader_id);

		glm::mat4 view = camera.view;
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

		glm::mat4 view = camera.view;
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

		// Camera view and projection matrices
		glm::mat4 view = camera.view;
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
