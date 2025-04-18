#pragma once

#include <bnp/components/graphics.h>

#include <unordered_map>
#include <GL/glew.h>

namespace bnp {

	class MaterialFactory {
	public:
		MaterialFactory() = default;

		Material load_material(const unordered_map<ShaderType, std::string>& shaders);
		Material wireframe_material();

	protected:
		GLuint compile_shader(const char* source, GLuint type);
	};

} // namespace bnp
