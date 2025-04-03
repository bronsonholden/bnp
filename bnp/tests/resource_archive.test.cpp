#include <bnp/core/scene.h>
#include <bnp/resources/scene_archive.hpp>

#include <entt/entt.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/string.h>
#include <fstream>
#include <filesystem>

TEST_CASE("resource archive saves and loads") {
	namespace fs = std::filesystem;
	entt::registry registry;

	fs::path path = fs::temp_directory_path() / "test_archive.bin";
	bnp::SceneArchive saving_archive(registry);
	bnp::Scene scene(registry);

	bnp::Node node = scene.create_node();
	node.add_component<bnp::Position>(bnp::Position{ { 1.0f, 2.0f, 3.0f } });

	saving_archive.insert("test_scene", &scene);
	saving_archive.save(path);

	bnp::SceneArchive loading_archive(registry);

	loading_archive.load(path);
	bnp::Scene* loaded_scene = loading_archive.find("test_scene");

	REQUIRE(loaded_scene != nullptr);

	bnp::Node loaded_node = loaded_scene->get_all_nodes().at(0);

	const auto& pos = loaded_node.get_component<bnp::Position>();
	REQUIRE(pos.value.x == Catch::Approx(1.0f));
	REQUIRE(pos.value.y == Catch::Approx(2.0f));
	REQUIRE(pos.value.z == Catch::Approx(3.0f));

	fs::remove(path);
}
