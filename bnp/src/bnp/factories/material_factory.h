#pragma once

#include <bnp/components/graphics.h>

#include <fstream>
#include <unordered_map>
#include <GL/glew.h>

namespace bnp {

	class MaterialFactory {
	public:
		MaterialFactory() = default;

		Material load_material_from_files(const unordered_map<ShaderType, std::filesystem::path>& shader_files);
		Material load_material(const unordered_map<ShaderType, std::string>& shaders);

	protected:
		GLuint compile_shader(const char* source, GLuint type);
	};

} // namespace bnp
