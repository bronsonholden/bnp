#pragma once

#include <bnp/resources/resource_archive.hpp>

namespace bnp {

	template <typename T, typename A = ResourceAllocator<T>>
	class ResourceArchive : public ResourceArchiveImpl<T, A> {
	};

	// specialization for Scene
	template<>
	class ResourceArchive<Scene> : public ResourceArchiveImpl<Scene, ResourceAllocator<Scene>> {
	public:
		ResourceArchive(entt::registry& _registry) : registry(_registry), ResourceArchiveImpl<Scene, ResourceAllocator<Scene>>(_registry)
		{
		}

	private:
		entt::registry& registry;
	};

	using SceneArchive = ResourceArchive<Scene, ResourceAllocator<Scene>>;

} // namespace bnp
