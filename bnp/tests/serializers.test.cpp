#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/string.h>

#include <bnp/core/node.hpp>
#include <bnp/core/window.h>
#include <bnp/serializers/transform.hpp>
#include <bnp/serializers/node.hpp>
#include <bnp/serializers/scene.hpp>
#include <bnp/serializers/graphics.hpp>

#include <bnp/managers/archive_manager.h>

#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>

TEST_CASE("transform serialization") {
	namespace fs = std::filesystem;

	// Create a temporary file
	const fs::path path = fs::temp_directory_path() / "test_position.bin";

	// Original component
	bnp::Transform original{ { 1.0f, 2.0f, 3.0f } };

	CHECK(original.position.x == Catch::Approx(1.0f));
	CHECK(original.position.y == Catch::Approx(2.0f));
	CHECK(original.position.z == Catch::Approx(3.0f));

	// Serialize to file
	{
		std::ofstream os(path, std::ios::binary);
		REQUIRE(os.is_open());

		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
		ser.object(original);
		ser.adapter().flush();
	}

	// Deserialize from file
	bnp::Transform loaded{};
	{
		std::ifstream is(path, std::ios::binary);
		REQUIRE(is.is_open());

		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };
		des.object(loaded);
		auto status = des.adapter().error();
		REQUIRE(status == bitsery::ReaderError::NoError);
	}

	// Compare
	CHECK(loaded.position.x == Catch::Approx(1.0f));
	CHECK(loaded.position.y == Catch::Approx(2.0f));
	CHECK(loaded.position.z == Catch::Approx(3.0f));

	// Cleanup
	fs::remove(path);
}

TEST_CASE("node serialization") {
	namespace fs = std::filesystem;

	// Create a temporary file
	const fs::path path = fs::temp_directory_path() / "test_node.bin";

	// Create registry and original node
	entt::registry registry;
	bnp::Node original_node(registry);

	original_node.add_component<bnp::Transform>(bnp::Transform{ { 1.0f, 1.0f, 1.0f } });

	// Serialize to file
	{
		std::ofstream os(path, std::ios::binary);
		REQUIRE(os.is_open());

		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
		ser.object(original_node);
		ser.adapter().flush();
	}

	bnp::Node loaded_node(registry);
	// Deserialize into a new node
	{
		std::ifstream is(path, std::ios::binary);
		REQUIRE(is.is_open());

		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };
		des.object(loaded_node);
		auto status = des.adapter().error();
		REQUIRE(status == bitsery::ReaderError::NoError);
	}

	// Validate that the loaded node has the correct position
	REQUIRE(registry.all_of<bnp::Transform>(loaded_node.get_entity_id()));
	const auto& transform = registry.get<bnp::Transform>(loaded_node.get_entity_id());

	CHECK(transform.position.x == Catch::Approx(1.0f));
	CHECK(transform.position.y == Catch::Approx(1.0f));
	CHECK(transform.position.z == Catch::Approx(1.0f));

	// Cleanup
	fs::remove(path);
}

TEST_CASE("instances serialization") {
	namespace fs = std::filesystem;

	// Create a temporary file
	const fs::path path = fs::temp_directory_path() / "test_instances.bin";

	bnp::Instances instances;
	bnp::Window window;

	instances.transforms.push_back(bnp::Transform{ glm::vec3(1.0f, 2.0f, 3.0f), glm::quat(), glm::vec3(1.0f, 2.0f, 3.0f) });
	instances.update_transforms();

	// Serialize to file
	{
		std::ofstream os(path, std::ios::binary);
		REQUIRE(os.is_open());

		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
		ser.object(instances);
		ser.adapter().flush();
	}

	bnp::Instances loaded_instances;
	// Deserialize into a new node
	{
		std::ifstream is(path, std::ios::binary);
		REQUIRE(is.is_open());

		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };
		des.object(loaded_instances);
		auto status = des.adapter().error();
		REQUIRE(status == bitsery::ReaderError::NoError);
	}

	const bnp::Transform transform = loaded_instances.transforms.at(0);
	CHECK(transform.position.x == Catch::Approx(1.0f));
	CHECK(transform.position.y == Catch::Approx(2.0f));
	CHECK(transform.position.z == Catch::Approx(3.0f));

	// Cleanup
	fs::remove(path);
}

TEST_CASE("scene serialization") {
	entt::registry registry;
	namespace fs = std::filesystem;

	// Create a temporary file
	const fs::path path = fs::temp_directory_path() / "test_scene.bin";

	// Create original scene
	bnp::Scene original_scene(registry);

	// Add nodes to the scene
	bnp::Node node1 = original_scene.create_node();
	bnp::Node node2 = original_scene.create_node();

	// Add components to nodes
	node1.add_component<bnp::Transform>(bnp::Transform{ { 1.0f, 2.0f, 3.0f } });
	node2.add_component<bnp::Transform>(bnp::Transform{ { 4.0f, 5.0f, 6.0f } });

	std::vector<bnp::Node> node_list;

	// Serialize the scene to a file
	{
		std::ofstream os(path, std::ios::binary);
		REQUIRE(os.is_open());

		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
		ser.object(original_scene);
		ser.adapter().flush();

	}

	// Create an empty scene to load into
	bnp::Scene loaded_scene(registry);

	// Deserialize the scene from the file
	{
		std::ifstream is(path, std::ios::binary);
		REQUIRE(is.is_open());

		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };
		des.object(loaded_scene);
		auto status = des.adapter().error();
		REQUIRE(status == bitsery::ReaderError::NoError);
	}

	// Validate that the loaded scene has the correct nodes and components;
	std::vector<bnp::Node> loaded_nodes;

	for (const auto& [_, node] : loaded_scene.get_nodes()) {
		loaded_nodes.push_back(node);
	}

	REQUIRE(loaded_nodes.size() == 2);

	bnp::Node node3 = loaded_nodes.at(0);
	bnp::Node node4 = loaded_nodes.at(1);

	REQUIRE(node3.has_component<bnp::Transform>());
	REQUIRE(node4.has_component<bnp::Transform>());

	const auto& transform1 = node3.get_component<bnp::Transform>();
	CHECK(transform1.position.x == Catch::Approx(1.0f));
	CHECK(transform1.position.y == Catch::Approx(2.0f));
	CHECK(transform1.position.z == Catch::Approx(3.0f));

	const auto& transform2 = node4.get_component<bnp::Transform>();
	CHECK(transform2.position.x == Catch::Approx(4.0f));
	CHECK(transform2.position.y == Catch::Approx(5.0f));
	CHECK(transform2.position.z == Catch::Approx(6.0f));

	// Cleanup the temporary file
	fs::remove(path);
}

TEST_CASE("archive index serialization") {
	namespace fs = std::filesystem;

	std::filesystem::path path = fs::temp_directory_path() / "test_archive_index.bin";
	bnp::ArchiveIndex index;

	index.entries.emplace("test_resource_file.bin", bnp::ArchiveIndexEntry{
		{ "test_resource", { 0, 100 } }
		});

	{
		std::ofstream os(path, std::ios::binary);
		REQUIRE(os.is_open());

		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };

		ser.object(index);
		ser.adapter().flush();
	}

	bnp::ArchiveIndex loaded_index;
	{
		std::ifstream is(path, std::ios::binary);
		REQUIRE(is.is_open());

		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

		des.object(loaded_index);
	}

	REQUIRE(loaded_index.entries.size() == 1);
	REQUIRE(loaded_index.entries.at("test_resource_file.bin").at("test_resource").first == 0);
	REQUIRE(loaded_index.entries.at("test_resource_file.bin").at("test_resource").second == 100);

	fs::remove(path);
}


TEST_CASE("collect component IDs") {
	std::vector<uint32_t> component_ids;
	entt::registry registry;
	entt::entity entity = registry.create();

	registry.emplace<bnp::Transform>(entity, bnp::Transform{ {1.0f, 1.0f, 1.0f } });

	bnp::collect_component_ids<bnp::Transform>(registry, entity, component_ids);

	REQUIRE(component_ids[0] == entt::type_hash<bnp::Transform>::value());
}


TEST_CASE("get transform") {
	std::vector<uint32_t> component_ids;
	entt::registry registry;
	entt::entity entity = registry.create();

	registry.emplace<bnp::Transform>(entity, bnp::Transform{ {1.0f, 1.0f, 1.0f } });

	bnp::Transform transform = registry.get<bnp::Transform>(entity);

	CHECK(transform.position.x == Catch::Approx(1.0f));
	CHECK(transform.position.y == Catch::Approx(1.0f));
	CHECK(transform.position.z == Catch::Approx(1.0f));
}
