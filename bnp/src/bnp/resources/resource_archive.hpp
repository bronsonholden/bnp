#pragma once

#include <bnp/serializers/scene.hpp>
#include <bnp/resources/allocator.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>

namespace bnp {

template <typename T, typename A = DefaultResourceAllocator<T>>
class ResourceArchiveImpl {
public:
	ResourceArchiveImpl(A allocator = A()) : resource_allocator(allocator) {}

	void insert(const std::string& resource_name, T* resource) {
		resources.emplace(resource_name, resource);
	}

	std::unique_ptr<T> retrieve(const std::string& name) {
		return resources.at(name);
	}

	void save(std::filesystem::path path) {
		std::ofstream os(path, std::ios::binary);
		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };

		uint64_t resource_count = resources.size();

		ser.value8b(resource_count);

		for (const auto& [key, resource] : resources) {
			ser.text1b(key, 256);
			ser.object(*resource);
		}

		os.close();
	}

	void load(std::filesystem::path path) {
		std::ifstream is(path, std::ios::binary);
		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

		uint64_t resource_count;

		des.value8b(resource_count);

		for (uint64_t i = 0; i < resource_count; ++i) {
			std::string key;
			T* resource = resource_allocator.allocate();
			des.text1b(key, 256);
			des.object(*resource);

			auto status = des.adapter().error();
			if (status == bitsery::ReaderError::NoError) {
				resources.emplace(key, resource);
			}
			else {
				delete resource;
			}
		}
	}

	T* find(std::string key) {
		return resources[key];
	}

protected:
	A resource_allocator;

	std::unordered_map<std::string, T*> resources;
	std::unordered_map<std::string, uint64_t> index;
	std::filesystem::path file_path;
};

} // namespace bnp
