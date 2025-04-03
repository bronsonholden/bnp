#pragma once

/* ResourceManager
 *
 * Stores generated meshes, materials, and other resources and cleans up them
 * up via `cleanup()` only if the given registry contains no components that
 * have them attached.
 */

#include <bnp/components/graphics.h>

#include <entt/entt.hpp>
#include <vector>

namespace bnp {

	class ResourceManager {
	public:
		ResourceManager() = default;

		void add_mesh(Mesh mesh);
		void add_material(Material material);
		void cleanup(const entt::registry& registry);

	protected:
		std::vector<Mesh> meshes;
		std::vector<Material> materials;

		void cleanup_meshes(const entt::registry& registry);
		void cleanup_materials(const entt::registry& registry);
	};

} // namespace bnp
