#include <bnp/factories/material_factory.h>

#include <iostream>

const char* obstacle_vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* obstacle_fragment_shader_source = R"(
#version 330 core

out vec4 FragColor;

void main() {
    FragColor = vec4(1.0);
}
)";

const char* quad_vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 _aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    // quads are -0.5, -0.5 to 0.5, 0.5 so scale it up
    gl_Position = vec4(aPos.xy * 2, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* quad_fragment_shader_source = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main() {
    FragColor = texture(screenTexture, TexCoord);
}
)";

const char* wireframe_vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

out vec3 frag_pos;
out vec4 frag_color;

void main() {
    frag_pos = vec3(model * vec4(aPos, 1.0));
    frag_color = color;
    gl_Position = projection * view * vec4(frag_pos, 1.0);
}
)";

const char* wireframe_fragment_shader_source = R"(
#version 330 core

out vec4 out_frag_color;
in vec3 frag_pos;
in vec4 frag_color;

void main() {
    out_frag_color = frag_color;
}
)";

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

	Material MaterialFactory::wireframe_material() {
		return load_material({
			{ShaderType::VertexShader, wireframe_vertex_shader_source},
			{ShaderType::FragmentShader, wireframe_fragment_shader_source},
			});
	}

	Material MaterialFactory::quad_material() {
		return load_material({
			{ShaderType::VertexShader, quad_vertex_shader_source},
			{ShaderType::FragmentShader, quad_fragment_shader_source},
			});
	}

	Material MaterialFactory::obstacle_material() {
		return load_material({
			{ShaderType::VertexShader, obstacle_vertex_shader_source},
			{ShaderType::FragmentShader, obstacle_fragment_shader_source},
			});
	}

} // namespace bnp
