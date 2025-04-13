#pragma once

/* ResourceManager
 *
 * Stores generated meshes, materials, and other resources and cleans up them
 * up via `cleanup()` only if the given registry contains no components that
 * have them attached.
 */

#include <bnp/components/graphics.h>
#include <bnp/managers/archive_manager.h>

#include <entt/entt.hpp>
#include <vector>

namespace bnp {

	class ResourceManager {
	public:
		ResourceManager() = default;

		void add_mesh(ResourceIdentifier resource_id, Mesh mesh);
		void add_material(ResourceIdentifier resource_id, Material material);
		void add_texture(ResourceIdentifier resource_id, Texture  texture);
		void cleanup(const entt::registry& registry);

	protected:
		std::unordered_map<ResourceIdentifier, Mesh> meshes;
		std::unordered_map<ResourceIdentifier, Material> materials;
		std::unordered_map<ResourceIdentifier, Texture> textures;

		void cleanup_meshes(const entt::registry& registry);
		void cleanup_materials(const entt::registry& registry);
		void cleanup_textures(const entt::registry& registry);
	};

} // namespace bnp
