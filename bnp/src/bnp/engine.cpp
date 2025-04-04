#include <bnp/engine.h>

#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>
#include <bnp/helpers/random_float_generator.hpp>
#include <bnp/serializers/scene.hpp>

#include <string>
#include <fstream>
#include <filesystem>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>

#include <iostream>
using namespace std;

// Basic vertex shader source code
const char* vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Instanced model matrix
layout (location = 3) in mat4 model;

uniform mat4 view;
uniform mat4 projection;

out vec3 frag_pos;
out vec3 normal;
out vec2 tex_coords;

void main() {
    frag_pos = vec3(model * vec4(aPos, 1.0));
    normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    tex_coords = aTexCoords;
    gl_Position = projection * view * vec4(frag_pos, 1.0);
}
)";

// Basic fragment shader source code
const char* fragment_shader_source = R"(
#version 330 core
out vec4 frag_color;

in vec3 frag_pos;
in vec3 normal;
in vec2 tex_coords;

uniform vec3 color;

void main() {
    vec3 light_dir = normalize(vec3(2.0, 1.0, 0.3));
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * color;
    frag_color = vec4(diffuse, 1.0);
}
)";

namespace bnp {

	Engine::Engine() {
		renderer.initialize();
	}

	Engine::~Engine() {
		renderer.shutdown();
	}

	void Engine::create_test_scene_data_file() {
		Scene scene(registry);

		Material material = material_factory.load_material({
			{GL_VERTEX_SHADER, vertex_shader_source},
			{GL_FRAGMENT_SHADER, fragment_shader_source}
			});

		Mesh mesh = factory.cube(1.0f);

		Node cube = scene.create_node();
		cube.add_component<Position>(Position{ { 0.0f, 0.0f, 0.0f } });
		cube.add_component<Mesh>(mesh);
		cube.add_component<Material>(material);
		cube.add_component<Renderable>(true);

		Instances instances;

		const int count = 100000;

		instances.positions.reserve(count);
		instances.rotations.reserve(count);
		instances.scales.reserve(count);

		RandomFloatGenerator pos_gen(-50.0f, 50.0f);
		RandomFloatGenerator rot_gen(-1.0f, 1.0f);
		RandomFloatGenerator scale_gen(0.1, 0.8f);

		for (int i = 0; i < count; ++i) {
			instances.positions.push_back(Position{ { pos_gen.generate(), pos_gen.generate(), pos_gen.generate() } });
			instances.rotations.push_back(Rotation{ { glm::quat() } });
			instances.scales.push_back(Scale{ glm::vec3(scale_gen.generate()) });
		}

		instances.update_transforms();
		cube.add_component<Instances>(instances);

		resource_manager.add_mesh(mesh);

		namespace fs = std::filesystem;
		const fs::path root = PROJECT_ROOT;
		const fs::path path = root / "bnp/data/test_instances.bin";
		std::ofstream os(path, std::ios::binary);
		bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
		ser.object(scene);
		ser.adapter().flush();
	}

	void Engine::load_test_scene_data_file(Scene& scene) {
		namespace fs = std::filesystem;

		const fs::path root = PROJECT_ROOT;
		const fs::path path = root / "bnp/data/test_instances.bin";
		std::ifstream is(path, std::ios::binary);
		bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };
		des.object(scene);
	}

	void Engine::run() {
		const bool load = true;

		Material material = material_factory.load_material({
			{GL_VERTEX_SHADER, vertex_shader_source},
			{GL_FRAGMENT_SHADER, fragment_shader_source}
			});

		Mesh mesh = factory.cube(1.0f);

		resource_manager.add_mesh(mesh);
		resource_manager.add_material(material);

		Scene scene(registry);

		if (!load) {
			create_test_scene_data_file();
		}
		else {
			load_test_scene_data_file(scene);

			cout << "Scene loaded" << endl;
			cout << "Nodes: " << scene.get_all_nodes().size() << endl;

			for (auto& node : scene.get_all_nodes()) {
				node.add_component<Mesh>(mesh);
				node.add_component<Material>(material);
				node.get_component<Renderable>().value = true;

				cout << "Node " << ((int)node.get_entity_id()) << ": " << node.get_num_components() << " components" << endl;
				cout << "  EnTT valid: " << node.get_registry().valid(node.get_entity_id()) << endl;
				cout << "  Position: " << node.has_component<Position>() << endl;
				cout << "  Instances: " << node.has_component<Instances>() << endl;
				cout << "    Instance count: " << node.get_component<Instances>().positions.size() << endl;
				cout << "    Instances component ptr: " << &node.get_component<Instances>() << endl;
				cout << "  Material: " << node.has_component<Material>() << endl;
				cout << "  Mesh: " << node.has_component<Mesh>() << endl;
				cout << "  Renderable: " << node.has_component<Renderable>() << endl;
				cout << "    value: " << node.get_component<Renderable>().value << endl;
			}
		}

		auto view = registry.view<Mesh, Material, Renderable, Instances>();


		float accumulator = 0.0f;
		const float fixed_dt = 1.0f / 60.0f;

		time.start();

		Camera camera({
			glm::vec3(5.0f, 5.0f, 5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			});

		while (window.open) {
			window.poll();

			time.new_frame();

			float dt = time.delta_time();

			while (time.needs_fixed_update()) {
				fixed_update();
				time.consume_fixed_step();
			}

			//cout << "fps: " << std::to_string(1.0f / dt) << endl;

			// manager updates

			window.clear();

			// rendering
			//render_manager.render(registry, renderer);
			render_manager.render_instances(registry, renderer);

			window.swap();
		}

		resource_manager.cleanup(registry);
	}

	void Engine::update(float dt) {

	}

	void Engine::fixed_update() {

	}
}
