#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>

#include <bnp/core/node.hpp>
#include <bnp/serializers/transform.hpp>
#include <bnp/serializers/node.hpp>

#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>

TEST_CASE("position serialization") {
	namespace fs = std::filesystem;

	// Create a temporary file
	const fs::path path = fs::temp_directory_path() / "test_position.bin";

	// Original component
	bnp::Position original{ { 1.0f, 2.0f, 3.0f } };

	CHECK(original.value.x == Catch::Approx(1.0f));
	CHECK(original.value.y == Catch::Approx(2.0f));
	CHECK(original.value.z == Catch::Approx(3.0f));

	// Serialize to file
	{
		std::ofstream os(path, std::ios::binary);
		REQUIRE(os.is_open());

		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
		ser.object(original);
		ser.adapter().flush();
	}

	// Deserialize from file
	bnp::Position loaded{};
	{
		std::ifstream is(path, std::ios::binary);
		REQUIRE(is.is_open());

		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };
		des.object(loaded);
		auto status = des.adapter().error();
		REQUIRE(status == bitsery::ReaderError::NoError);
	}

	// Compare
	CHECK(loaded.value.x == Catch::Approx(1.0f));
	CHECK(loaded.value.y == Catch::Approx(2.0f));
	CHECK(loaded.value.z == Catch::Approx(3.0f));

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

	original_node.add_component<bnp::Position>(bnp::Position{ { 1.0f, 1.0f, 1.0f } });

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
	REQUIRE(registry.all_of<bnp::Position>(loaded_node.get_entity_id()));
	const auto& pos = registry.get<bnp::Position>(loaded_node.get_entity_id());

	CHECK(pos.value.x == Catch::Approx(1.0f));
	CHECK(pos.value.y == Catch::Approx(1.0f));
	CHECK(pos.value.z == Catch::Approx(1.0f));

	// Cleanup
	fs::remove(path);
}

TEST_CASE("collect component IDs") {
	std::vector<uint32_t> component_ids;
	entt::registry registry;
	entt::entity entity = registry.create();

	registry.emplace<bnp::Position>(entity, bnp::Position{ {1.0f, 1.0f, 1.0f } });

	collect_component_ids<bnp::Position>(registry, entity, component_ids);

	REQUIRE(component_ids[0] == entt::type_hash<bnp::Position>::value());
}


TEST_CASE("get position") {
	std::vector<uint32_t> component_ids;
	entt::registry registry;
	entt::entity entity = registry.create();

	registry.emplace<bnp::Position>(entity, bnp::Position{ {1.0f, 1.0f, 1.0f } });

	bnp::Position position = registry.get<bnp::Position>(entity);

	CHECK(position.value.x == Catch::Approx(1.0f));
	CHECK(position.value.y == Catch::Approx(1.0f));
	CHECK(position.value.z == Catch::Approx(1.0f));
}
