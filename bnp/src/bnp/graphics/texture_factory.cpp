#include <bnp/graphics/texture_factory.h>

#include <stb_image.h>

namespace bnp {

	Texture TextureFactory::load_from_file(std::filesystem::path path) {
		Texture texture;

		int width, height, channels;
		unsigned char* data = stbi_load(path.string().data(), &width, &height, &channels, 0);

		GLenum format;
		if (channels == 1) format = GL_RED;
		else if (channels == 3) format = GL_RGB;
		else if (channels == 4) format = GL_RGBA;
		else {
			stbi_image_free(data);
			return texture;
		}

		glGenTextures(1, &texture.texture_id);
		glBindTexture(GL_TEXTURE_2D, texture.texture_id);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			format,
			width,
			height,
			0,
			format,
			GL_UNSIGNED_BYTE,
			data
		);

		// todo: add blend mode enum in Texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);

		return texture;
	}

}
