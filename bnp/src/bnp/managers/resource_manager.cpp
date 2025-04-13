#include <bnp/managers/resource_manager.h>

#include <iostream>
using namespace std;

namespace bnp {

	void ResourceManager::add_mesh(ResourceIdentifier resource_id, Mesh mesh) {
		meshes.emplace(resource_id, mesh);
	}

	void ResourceManager::add_material(ResourceIdentifier resource_id, Material material) {
		materials.emplace(resource_id, material);
	}

	void ResourceManager::add_texture(ResourceIdentifier resource_id, Texture  texture) {
		textures.emplace(resource_id, texture);
	}

	void ResourceManager::cleanup(const entt::registry& registry) {
		cleanup_meshes(registry);
		cleanup_materials(registry);
		cleanup_textures(registry);
	}

	void ResourceManager::cleanup_meshes(const entt::registry& registry) {
		auto meshes_view = registry.view<Mesh>();
		std::vector<GLuint> used_va_ids, used_vb_ids, unused_va_ids, unused_vb_ids;

		used_va_ids.reserve(512);
		used_vb_ids.reserve(512);

		for (auto& entity : meshes_view) {
			const auto& [mesh] = meshes_view.get(entity);
			used_va_ids.push_back(mesh.va_id);
			used_vb_ids.push_back(mesh.vb_id);
			used_vb_ids.push_back(mesh.eb_id);
		}


		for (const auto& [_, mesh] : meshes) {
			if (mesh.va_id && std::find(used_va_ids.begin(), used_va_ids.end(), mesh.va_id) == used_va_ids.end()) {
				// if va isn't found
				unused_va_ids.push_back(mesh.va_id);
			}
			if (mesh.vb_id && std::find(used_vb_ids.begin(), used_vb_ids.end(), mesh.vb_id) == used_vb_ids.end()) {
				// if vb isn't found
				unused_vb_ids.push_back(mesh.vb_id);
			}
			if (mesh.eb_id && std::find(used_vb_ids.begin(), used_vb_ids.end(), mesh.eb_id) == used_vb_ids.end()) {
				// if eb isn't found
				unused_vb_ids.push_back(mesh.eb_id);
			}
		}

		if (!unused_va_ids.empty()) {
			glDeleteVertexArrays(static_cast<GLsizei>(unused_va_ids.size()), unused_va_ids.data());
		}

		if (!unused_vb_ids.empty()) {
			glDeleteBuffers(static_cast<GLsizei>(unused_vb_ids.size()), unused_vb_ids.data());
		}
	}

	void ResourceManager::cleanup_materials(const entt::registry& registry) {
		auto materials_view = registry.view<Material>();
		std::vector<GLuint> used_shader_ids, unused_shader_ids;

		used_shader_ids.reserve(512);
		unused_shader_ids.reserve(512);

		for (auto& entity : materials_view) {
			const auto& [material] = materials_view.get(entity);
			used_shader_ids.push_back(material.shader_id);
		}


		for (const auto& [_, material] : materials) {
			if (std::find(used_shader_ids.begin(), used_shader_ids.end(), material.shader_id) == used_shader_ids.end()) {
				// if program isn't found
				unused_shader_ids.push_back(material.shader_id);
			}
		}

		for (GLuint program : unused_shader_ids) {
			GLint shaderCount = 0;

			glGetProgramiv(program, GL_ATTACHED_SHADERS, &shaderCount);

			if (shaderCount > 0) {
				std::vector<GLuint> shaders(shaderCount);
				glGetAttachedShaders(program, shaderCount, nullptr, shaders.data());

				for (GLuint shader : shaders) {
					glDetachShader(program, shader);
					glDeleteShader(shader);
				}
			}

			glDeleteProgram(program);
		}
	}

	void ResourceManager::cleanup_textures(const entt::registry& registry) {
		auto textures_view = registry.view<Texture>();
		std::vector<GLuint> used_texture_ids, unused_texture_ids;

		used_texture_ids.reserve(512);
		unused_texture_ids.reserve(512);

		for (auto& entity : textures_view) {
			const auto& [texture] = textures_view.get(entity);
			used_texture_ids.push_back(texture.texture_id);
		}


		for (const auto& [_, texture] : textures) {
			if (std::find(used_texture_ids.begin(), used_texture_ids.end(), texture.texture_id) == used_texture_ids.end()) {
				// if texture isn't found
				unused_texture_ids.push_back(texture.texture_id);
			}
		}

		for (GLuint texture_id : unused_texture_ids) {
			glDeleteTextures(1, &texture_id);
		}
	}

}
