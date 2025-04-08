#include <iostream>
#include <fstream>
#include <filesystem>
#include "testing.h"
#include <bnp/managers/archive_manager.h>

void printByte(std::byte b) {
	// Print the byte as a 2-digit hex number with leading zeros
	std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
		<< static_cast<int>(std::to_integer<uint8_t>(b)) << std::endl;
}

TEST_CASE("creates unique archive") {
	namespace fs = std::filesystem;
	std::filesystem::path dir = fs::temp_directory_path() / "data";
	bnp::ArchiveManager archive_manager(dir);

	REQUIRE(archive_manager.create_archive("test_archive.bin"));
	REQUIRE(!archive_manager.create_archive("test_archive.bin"));
	REQUIRE(archive_manager.get_archive("test_archive.bin") != nullptr);
}

TEST_CASE("adds resource to archive and index") {
	namespace fs = std::filesystem;
	std::filesystem::path dir = fs::temp_directory_path() / "data";
	bnp::ArchiveManager archive_manager(dir);

	REQUIRE(archive_manager.create_archive("test_archive.bin"));

	REQUIRE(archive_manager.add_resource(
		"test_archive.bin",
		"test_resource",
		bnp::ResourceData{
			{ std::byte{ 0x01 }, std::byte{ 0x02 } }
		}));

	const bnp::Resource* resource = archive_manager.get_resource("test_resource");

	REQUIRE(resource != nullptr);
	REQUIRE((resource->data.bytes == std::vector<std::byte>{ { std::byte{ 0x01 }, std::byte{ 0x02 } } }));
	REQUIRE(resource->pending);
}

TEST_CASE("copies archive index") {
	bnp::ArchiveIndex index;


	index.entries.emplace("test_archive.bin", bnp::ArchiveIndexEntry{
		{ "test_resource", bnp::ArchiveIndexSpan{ 0, 0 }}
		});

	bnp::ArchiveIndex new_index = index;

	REQUIRE(new_index.entries.size() == 1);
	REQUIRE(new_index.entries.at("test_archive.bin").size() == 1);
}

TEST_CASE("saves and loads archive and index files") {
	namespace fs = std::filesystem;
	std::filesystem::path dir = fs::temp_directory_path() / "data";
	std::filesystem::path index_path = dir / "index.bin";

	{
		bnp::ArchiveManager archive_manager(dir);

		REQUIRE(archive_manager.create_archive("test_archive.bin"));
		REQUIRE(archive_manager.get_archive("test_archive.bin") != nullptr);

		REQUIRE(archive_manager.add_resource(
			"test_archive.bin",
			"test_resource",
			bnp::ResourceData{
				{ std::byte{ 0x01 }, std::byte{ 0x02 } }
			}));

		archive_manager.save();

		// check that index file was created
		{
			std::ifstream is(index_path, std::ios::binary);
			REQUIRE(is.is_open());
		}

		bnp::ArchiveManager loaded_archive_manager(dir);
		loaded_archive_manager.load();

		const bnp::ArchiveIndex& loaded_index = loaded_archive_manager.get_index();

		// resource we added was pending, so it's not in the index
		REQUIRE(archive_manager.get_index().entries.size() == 0);
		REQUIRE(archive_manager.get_archive("test_archive.bin") != nullptr);
		// loaded manager loads from file, so it has the archive in its index
		REQUIRE(loaded_index.entries.size() == 1);
		// verify archive is in the index
		REQUIRE(loaded_index.entries.find("test_archive.bin") != loaded_index.entries.end());

		// verify index contents
		const bnp::ArchiveIndexEntry& entry = loaded_index.entries.at("test_archive.bin");
		REQUIRE(entry.size() == 1);
		REQUIRE(entry.find("test_resource") != entry.end());
		REQUIRE(entry.at("test_resource").first == 0);
		REQUIRE(entry.at("test_resource").second == 2);

		REQUIRE(!loaded_archive_manager.resource_is_loaded("test_resource"));
		const bnp::Resource* loaded_resource = loaded_archive_manager.get_resource("test_resource");
		REQUIRE(loaded_resource != nullptr);
		printByte(loaded_resource->data.bytes[0]);
		REQUIRE((loaded_resource->data.bytes == std::vector<std::byte>{ std::byte{ 0x01 }, std::byte{ 0x02 } }));
		REQUIRE(loaded_archive_manager.resource_is_loaded("test_resource"));
	}

	// test rewriting archive with new resource
	{
		bnp::ArchiveManager archive_manager(dir);

		archive_manager.load();

		REQUIRE(archive_manager.add_resource(
			"test_archive.bin",
			"test_resource_2",
			bnp::ResourceData{
				{ std::byte{ 0x03 }, std::byte{ 0x04 }, std::byte{ 0x05 } }
			}));

		archive_manager.save();

		bnp::ArchiveManager loaded_archive_manager(dir);
		loaded_archive_manager.load();

		const bnp::ArchiveIndex& loaded_index = loaded_archive_manager.get_index();

		// resource we added was pending, so only the initial should in the index
		REQUIRE(archive_manager.get_index().entries.size() == 1);
		REQUIRE(archive_manager.get_archive("test_archive.bin") != nullptr);
		// loaded manager loads from file, so it has the archive in its index
		REQUIRE(loaded_index.entries.size() == 1);
		// verify archive is in the index
		REQUIRE(loaded_index.entries.find("test_archive.bin") != loaded_index.entries.end());

		// verify index contents
		const bnp::ArchiveIndexEntry& entry = loaded_index.entries.at("test_archive.bin");

		REQUIRE(entry.size() == 2);
		REQUIRE(entry.find("test_resource") != entry.end());
		REQUIRE(entry.find("test_resource_2") != entry.end());
		REQUIRE(entry.at("test_resource").first == 0);
		REQUIRE(entry.at("test_resource").second == 2);
		REQUIRE(entry.at("test_resource_2").first == 2);
		REQUIRE(entry.at("test_resource_2").second == 3);

		REQUIRE(!loaded_archive_manager.resource_is_loaded("test_resource"));
		const bnp::Resource* loaded_resource = loaded_archive_manager.get_resource("test_resource");
		REQUIRE(loaded_resource != nullptr);
		REQUIRE((loaded_resource->data.bytes == std::vector<std::byte>{ std::byte{ 0x01 }, std::byte{ 0x02 } }));
		REQUIRE(loaded_archive_manager.resource_is_loaded("test_resource"));

		REQUIRE(!loaded_archive_manager.resource_is_loaded("test_resource_2"));
		const bnp::Resource* loaded_resource_2 = loaded_archive_manager.get_resource("test_resource_2");
		REQUIRE(loaded_resource_2 != nullptr);
		REQUIRE((loaded_resource_2->data.bytes == std::vector<std::byte>{ std::byte{ 0x03 }, std::byte{ 0x04 }, std::byte{ 0x05 } }));
		REQUIRE(loaded_archive_manager.resource_is_loaded("test_resource_2"));
	}
	// todo: test serializing archive that includes a
	// resource from a file (not loaded into memory)

	fs::remove(dir / "index.bin");
	fs::remove(dir / "test_archive.bin");
}
