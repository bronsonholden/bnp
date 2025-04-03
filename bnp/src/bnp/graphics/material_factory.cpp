#include <bnp/graphics/material_factory.h>

#include <iostream>

namespace bnp {

	GLuint MaterialFactory::compile_shader(const char* source, GLuint type) {
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);

		// Check compilation
		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char info_log[512];
			glGetShaderInfoLog(shader, 512, nullptr, info_log);
			std::cerr << "Shader compilation failed: " << info_log << std::endl;
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

	Material MaterialFactory::load_material(
		const std::vector<std::pair<GLuint, const char*>>& shaders)
	{
		GLuint program = glCreateProgram();
		std::vector<GLuint> shader_objects;

		for (const auto& [shader_type, shader_source] : shaders) {
			GLuint shader = compile_shader(shader_source, shader_type);
			if (shader) {
				glAttachShader(program, shader);
				shader_objects.push_back(shader);
			}
		}

		glLinkProgram(program);

		// Check linking status
		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			char info_log[512];
			glGetProgramInfoLog(program, 512, nullptr, info_log);
			std::cerr << "Shader program linking failed: " << info_log << std::endl;

			// Clean up shaders if linking fails
			for (GLuint shader : shader_objects) {
				glDeleteShader(shader);
			}
			glDeleteProgram(program);
			return { 0 };
		}

		// Clean up shaders after successful linking
		for (GLuint shader : shader_objects) {
			glDetachShader(program, shader);
			glDeleteShader(shader);
		}

		return { program };
	}

} // namespace bnp
