#pragma once

#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/array.h>
#include <unordered_map>
#include <fstream>
#include <filesystem>

#include <iostream>
using namespace std;

namespace bnp {

	//typedef std::vector<std::byte> ResourceData;
	typedef std::string ResourceIdentifier;
	typedef std::string ResourceFileName;
	// offset, count
	typedef std::pair<uint64_t, uint64_t> ArchiveIndexSpan;
	typedef std::unordered_map<ResourceIdentifier, ArchiveIndexSpan> ArchiveIndexEntry;

	class ResourceData {
	public:
		ResourceData(std::vector<std::byte> _bytes = {}) : bytes(_bytes) {

		}

		ResourceData(ResourceData&& other) noexcept
			: bytes(std::move(other.bytes)) {

		}

		std::vector<std::byte> bytes;
	};

	class ArchiveIndex {
	public:
		ArchiveIndex() = default;

		std::unordered_map<ResourceFileName, ArchiveIndexEntry> entries;
	};

	template <typename S>
	void serialize(S& s, ResourceData& data) {
		s.object(data.bytes);
	}


	template <typename S>
	void serialize(S& s, ArchiveIndex& index) {
		uint64_t count = index.entries.size();

		s.value8b(count);

		if (index.entries.size()) {
			for (auto& [_file_name, entry] : index.entries) {
				ResourceFileName file_name = _file_name;
				uint64_t entry_count = entry.size();

				s.text1b(file_name, 256);
				s.value8b(entry_count);

				for (auto& [_resource_id, span] : entry) {
					// workaround; unordered_map key is const, but we're not deserializing here
					ResourceIdentifier resource_id = _resource_id;
					s.text1b(resource_id, 256);

					auto& [offset, count] = span;

					s.value8b(offset);
					s.value8b(count);
				}
			}
		}
		else {
			index.entries.reserve(count);

			while (count--) {
				ResourceFileName file_name;
				uint64_t entry_count = 0;
				ArchiveIndexEntry entry;

				s.text1b(file_name, 256);
				s.value8b(entry_count);

				index.entries.emplace(file_name, entry);
				index.entries.at(file_name).reserve(entry_count);

				while (entry_count--) {
					ResourceIdentifier identifier;
					uint64_t offset = 0;
					uint64_t count = 0;

					s.text1b(identifier, 256);
					s.value8b(offset);
					s.value8b(count);

					index.entries.at(file_name).emplace(identifier, ArchiveIndexSpan{ offset, count });
				}
			}
		}
	}

	class Resource {
	public:
		Resource(ResourceData&& _data, bool _pending = false)
			: data(std::move(_data)),
			pending(_pending)
		{

		}

		Resource(Resource&& other) noexcept
			: data(std::move(other.data)),
			pending(other.pending)
		{

		}

		ResourceData data;
		bool pending;
	};

	class Archive {
	public:

		Archive(std::filesystem::path _path) : path(_path) {

		}

		Archive(Archive&& other) noexcept
			: resources(std::move(other.resources)),
			pending_resources(std::move(other.pending_resources)),
			path(other.path)
		{
		}

		// resources loaded into memory, not used for serializing
		std::unordered_map<ResourceIdentifier, Resource> resources;
		// resources to be serialized from memory instead of copied from file
		std::unordered_map<ResourceIdentifier, Resource> pending_resources;
		std::filesystem::path path;
	};

	template <typename S>
	void serialize(S& s, Archive& archive) {
	}

	//void buffer_from_file(std::ifstream& is, std::vector<char>& buffer, uint64_t offset, uint64_t count) {
	//	is.seekg(offset);
	//	is.read(buffer.data(), count);
	//}

	class ArchiveManager {
	public:
		ArchiveManager(std::filesystem::path dir);
		~ArchiveManager();

		bool has_archive(ResourceFileName file_name) {
			return index.entries.find(file_name) != index.entries.end();
		}

		void load() {
			std::filesystem::path path = dir / "index.bin";
			std::ifstream is(path, std::ios::binary);
			bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

			des.object(index);

			for (auto& [_file_name, entries] : index.entries) {
				std::string file_name = _file_name;
				archives.emplace(file_name, Archive(dir / file_name));
			}

		}

		void save(uint64_t max_chunk_size = 1024 * 1024) {
			// serialized as:
			// - 8b: <x>
			//   container (<x> bytes)
			// - 8b: <y>
			//   container (<y> bytes)

			// copy the index
			ArchiveIndex new_index = index;

			std::filesystem::create_directory(dir);

			// todo: only write archives with pending resources
			for (auto& [file_name, archive] : archives) {

				std::filesystem::path p(file_name);
				std::string new_name = p.stem().string() + ".tmp" + p.extension().string();
				std::filesystem::path in_path(dir / file_name);
				std::filesystem::path out_path(dir / new_name);
				std::ifstream is(in_path, std::ios::binary);
				std::ofstream os(out_path, std::ios::binary);
				bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
				bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

				// if any indexed resources
				if (new_index.entries.find(file_name) != new_index.entries.end()) {
					auto& entries = new_index.entries.at(file_name);

					// write indexed resources
					for (auto& [resource_id, span] : entries) {
						// if not pending
						if (archive.pending_resources.find(resource_id) == archive.pending_resources.end()) {
							uint64_t offset = span.first;
							uint64_t remaining = span.second;
							uint64_t chunks_read = 0;

							while (remaining > 0) {
								uint64_t chunk_size = max_chunk_size;

								if (chunk_size > remaining) {
									chunk_size = remaining;
								}

								std::vector<char> buffer(chunk_size);
								uint64_t chunk_offset = offset + chunks_read * max_chunk_size;

								is.seekg(chunk_offset);
								is.read(buffer.data(), chunk_size);

								if (chunk_size < max_chunk_size) {
									std::vector<char> last_chunk_buffer(buffer.begin(), buffer.begin() + chunk_size);
									os.write(last_chunk_buffer.data(), chunk_size);
								}
								else {
									os.write(buffer.data(), chunk_size);
								}

								chunks_read++;
								remaining -= chunk_size;
							}
						}
					}
				}
				else {
					new_index.entries.emplace(file_name, ArchiveIndexEntry{});
				}

				// now write pending resources
				for (auto& [_resource_id, resource] : archive.pending_resources) {
					std::streamoff off = os.tellp();

					ResourceIdentifier resource_id = _resource_id;
					uint64_t count = resource.data.bytes.size();

					ser.container1b(resource.data.bytes, count);

					ArchiveIndexSpan span{ off, count };

					new_index.entries.at(file_name).emplace(resource_id, span);
				}

				ser.adapter().flush();
			}

			// save index
			{
				std::filesystem::path path = dir / "index.tmp.bin";
				std::ofstream os(path);
				bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };

				ser.object(new_index);
				ser.adapter().flush();
			}

			// rename archives and index
			{
				for (auto& [file_name, _archive] : archives) {
					std::filesystem::path p(file_name);
					std::filesystem::path tmp_name = p.stem().string() + ".tmp" + p.extension().string();
					std::filesystem::path tmp_path = dir / tmp_name;
					std::filesystem::path tar_name = dir / file_name;
					std::filesystem::remove(tar_name);
					std::filesystem::rename(tmp_path, tar_name);
				}

				std::filesystem::path tmp_path = dir / "index.tmp.bin";
				std::filesystem::path tar_path = dir / "index.bin";
				std::filesystem::remove(tar_path);
				std::filesystem::rename(tmp_path, tar_path);
			}

		}

		bool create_archive(std::string file_name) {
			if (archives.find(file_name) != archives.end()) {
				return false;
			}

			archives.emplace(file_name, Archive(dir / file_name));

			return true;
		}

		Archive* get_archive(std::string file_name) {
			if (archives.find(file_name) != archives.end()) {
				return &archives.at(file_name);
			}
			else {
				return nullptr;
			}
		}

		// load resource into memory
		const Resource* load_resource(std::string resource_id) {
			Archive* archive = get_archive_for_resource_id(resource_id);
			std::filesystem::path path = std::filesystem::absolute(archive->path);
			std::filesystem::path base = std::filesystem::absolute(dir);
			std::filesystem::path relative = std::filesystem::relative(path, base);
			std::ifstream is(path, std::ios::binary);

			bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

			auto& span = index.entries.at(relative.string()).at(resource_id);

			uint64_t offset = span.first;
			uint64_t num_bytes = span.second;

			ResourceData data{ std::vector<std::byte>(num_bytes) }; // todo

			is.seekg(span.first);

			des.container1b(data.bytes, num_bytes);

			auto result = archive->resources.emplace(resource_id, Resource{ std::move(data), false });

			if (result.second) {
				return &result.first->second;
			}
			else {
				return nullptr;
			}

		}

		bool resource_is_loaded(ResourceIdentifier resource_id) {
			Archive* archive = get_archive_for_resource_id(resource_id);

			if (archive == nullptr) {
				return false;
			}

			return archive->resources.find(resource_id) != archive->resources.end();
		}

		// retrieve resource by resource ID (loads if not already loaded into archive)
		const Resource* get_resource(std::string resource_id) {
			Archive* archive = get_archive_for_resource_id(resource_id);

			if (!archive) {
				return nullptr;
			}

			archive->pending_resources.find(resource_id);
			archive->pending_resources.end();

			if (archive->pending_resources.find(resource_id) != archive->pending_resources.end()) {
				return &archive->pending_resources.at(resource_id);
			}
			else if (archive->resources.find(resource_id) != archive->resources.end()) {
				return &archive->resources.at(resource_id);
			}
			else {
				return load_resource(resource_id);
			}
		}

		// add resource to archive (if doesn't exist in any archive)
		bool add_resource(std::string file_name, std::string resource_id, ResourceData&& data) {
			Archive* archive = get_archive(file_name);

			if (!archive) {
				return false;
			}

			// check index, error if resource already exists by ID

			auto result = archive->pending_resources.emplace(resource_id, Resource{ std::move(data), true });

			// index is just for resources in files. may delete this
			//if (result.second) {
			//	//// update index
			//	//index.at(file_name).emplace(resource_id, { 0, 0 } });
			//	index.at(file_name).emplace(resource_id, { 0, 0 });
			//}

			return result.second;
		}

		// update resource
		bool update_resource(ResourceIdentifier resource_id, ResourceData&& data) {
			Archive* archive = get_archive_for_resource_id(resource_id);

			if (archive == nullptr) {
				return false;
			}

			archive->resources.emplace(resource_id, Resource{ std::move(data), true });
		}

		bool has_resource(ResourceIdentifier resource_id) {
			return get_archive_for_resource_id(resource_id) != nullptr;
		}

		// find archive by resource ID using index
		Archive* get_archive_for_resource_id(ResourceIdentifier resource_id) {
			// check pending resources (if updated) or loaded resources
			for (auto& [file_name, archive] : archives) {
				if (
					archive.pending_resources.find(resource_id) != archive.pending_resources.end()
					|| archive.resources.find(resource_id) != archive.resources.end()
					)
				{
					return &archive;
				}
			}

			// now check index in case resource isn't loaded
			for (auto& [file_name, entries] : index.entries) {
				if (entries.find(resource_id) != entries.end()) {
					return get_archive(file_name);
				}
			}

			return nullptr;
		}

		const ArchiveIndex& get_index() const {
			return index;
		}

	protected:
		ArchiveIndex index;
		// map of file_name and archives
		std::unordered_map<ResourceFileName, Archive> archives;
		std::filesystem::path dir;
	};

}
