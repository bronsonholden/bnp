#include <bnp/core/logger.hpp>
#include <bnp/components/graphics.h>

namespace bnp {

	void Material::cleanup() {
		GLint shader_count = 0;
		glGetProgramiv(shader_id, GL_ATTACHED_SHADERS, &shader_count);

		std::vector<GLuint> shaders(shader_count);
		glGetAttachedShaders(shader_id, shader_count, &shader_count, shaders.data());

		if (shader_count != shaders.size()) {
			Log::warning("Attached shader count mismatch: expected %d, got %d", shaders.size(), shader_count);
		}

		for (GLuint shader : shaders) glDeleteShader(shader);
		glDeleteProgram(shader_id);

		Log::info("Deleted shader program %d", shader_id);

		shader_id = 0;
	}

}
