#include <bnp/factories/material_factory.h>

#include <iostream>
#include <filesystem>

namespace bnp {

GLuint gl_shader_type(ShaderType shader_type) {
	switch (shader_type) {
	case VertexShader: return GL_VERTEX_SHADER;
	case FragmentShader: return GL_FRAGMENT_SHADER;
	default:
		return 0;
	}
}

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

Material MaterialFactory::load_material_from_files(const unordered_map<ShaderType, std::filesystem::path>& shader_files) {
	std::unordered_map<ShaderType, std::string> sources;

	for (auto [type, resource_path] : shader_files) {
		std::filesystem::path root = std::filesystem::path(PROJECT_ROOT) / "bnp";
		std::filesystem::path path = root / resource_path;
		std::ifstream file(path);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to shader file: " + path.string());
		}

		std::ostringstream ss;
		ss << file.rdbuf();
		std::string source = ss.str();

		sources.emplace(type, source);
	}

	return load_material(sources);
}

Material MaterialFactory::load_material(
	const unordered_map<ShaderType, std::string>& shaders)
{
	GLuint program = glCreateProgram();
	std::vector<GLuint> shader_objects;

	for (const auto& [type, shader_source] : shaders) {
		GLuint shader_type = gl_shader_type(type);
		GLuint shader = compile_shader(shader_source.data(), shader_type);
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
		Material material;
		material.shader_id = 0;
		return material;
	}

	// Clean up shaders after successful linking
	for (GLuint shader : shader_objects) {
		glDetachShader(program, shader);
		glDeleteShader(shader);
	}

	Material material;
	material.shader_id = program;
	return material;
}

} // namespace bnp
