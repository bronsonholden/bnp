#pragma once

/* ResourceManager
 *
 * Stores generated meshes, materials, and other resources and cleans up them
 * up via `cleanup()`.
 */

#include <bnp/components/graphics.h>
#include <bnp/managers/archive_manager.h>

#include <entt/entt.hpp>
#include <unordered_map>

namespace bnp {

class ResourceManager {
public:
	ResourceManager();
	~ResourceManager();

	void cleanup();

	Material load_material(ResourceIdentifier resource_id, std::unordered_map<ShaderType, std::filesystem::path> shaders);
	Texture load_texture(ResourceIdentifier resource_id, std::filesystem::path path);

protected:
	std::filesystem::path root;
	std::unordered_map<ResourceIdentifier, Material> materials;
	std::unordered_map<ResourceIdentifier, Texture> textures;
};

} // namespace bnp
