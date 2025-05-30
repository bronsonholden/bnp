#pragma once

namespace bnp {

template <typename T>
class ResourceAllocatorImpl {
public:
	ResourceAllocatorImpl() { }

	virtual T* allocate() = 0;
};

template <typename T>
class DefaultResourceAllocator {
public:
	DefaultResourceAllocator() { }

	T* allocate() {
		return new T();
	}
};

template <typename T>
class ResourceAllocator : public ResourceAllocatorImpl<T> {
};

template <>
class ResourceAllocator<Scene> : public ResourceAllocatorImpl<Scene> {
public:
	ResourceAllocator(entt::registry& _registry) : registry(_registry) {
	}

	Scene* allocate() override {
		return new Scene(registry);
	}

protected:
	entt::registry& registry;
};

} // namespace bnp
