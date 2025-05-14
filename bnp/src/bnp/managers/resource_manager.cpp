#include <bnp/managers/resource_manager.h>

#include <bnp/factories/material_factory.h>
#include <bnp/factories/texture_factory.h>

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
using namespace std;

namespace bnp {

	ResourceManager::ResourceManager()
		: root(std::filesystem::path(PROJECT_ROOT) / "bnp")
	{

	}

	ResourceManager::~ResourceManager() {

	}

	void ResourceManager::cleanup() {
		for (auto [resource_id, material] : materials) {
			material.cleanup();
		}
	}

	Material ResourceManager::load_material(ResourceIdentifier resource_id, std::unordered_map<ShaderType, std::filesystem::path> shaders) {
		if (materials.find(resource_id) != materials.end()) {
			return materials.at(resource_id);
		}

		std::unordered_map<ShaderType, std::string> source_map;

		for (auto& [shader_type, path] : shaders) {
			std::ifstream file(root / path);
			std::string source(std::istreambuf_iterator<char>(file), {});

			source_map.emplace(shader_type, std::move(source));
		}

		MaterialFactory material_factory;
		Material material = material_factory.load_material(source_map);

		materials.emplace(resource_id, material);

		return material;
	}

	Texture ResourceManager::load_texture(ResourceIdentifier resource_id, std::filesystem::path path) {
		if (textures.find(resource_id) != textures.end()) {
			return textures.at(resource_id);
		}

		TextureFactory texture_factory;
		Texture texture = texture_factory.load_from_file(root / path);

		texture.resource_id = resource_id;

		textures.emplace(resource_id, texture);

		return texture;
	}

}
