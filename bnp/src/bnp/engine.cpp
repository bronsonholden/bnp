#include <bnp/engine.h>

#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>
#include <bnp/helpers/random_float_generator.hpp>
#include <bnp/serializers/scene.hpp>
#include <bnp/managers/archive_manager.h>

#include <string>
#include <fstream>
#include <filesystem>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>

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

	Engine::Engine()
		: archive_manager(std::filesystem::path(PROJECT_ROOT) / "bnp/data"),
		test_scene(registry)
	{
		renderer.initialize();
		archive_manager.load();
	}

	Engine::~Engine() {
		renderer.shutdown();
	}

	void Engine::create_instanced_cubes_test_scene() {
		if (!archive_manager.has_archive("test_data.bin")) {
			archive_manager.create_archive("test_data.bin");
		}

		if (!archive_manager.has_resource("scene_test_instanced_cubes")) {
			MeshFactory mesh_factory;
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			// create mesh data and write
			{
				mesh_factory.create_cube(vertices, indices, 1.0f);

				std::vector<std::byte> buffer(
					sizeof(Vertex) * vertices.size()
					+ sizeof(uint32_t) * indices.size()
				);

				bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<std::byte>>> ser{ buffer };
				ser.container(vertices, vertices.size());
				ser.container4b(indices, indices.size());
				ser.adapter().flush();

				archive_manager.add_resource("test_data.bin", "scene_test_instanced_cubes", ResourceData{ buffer });

			}

			// create vertex shader
			{
				std::string vertex_source = vertex_shader_source;
				std::vector<std::byte> buffer(vertex_source.size() + sizeof(ShaderType));
				bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<std::byte>>> ser{ buffer };
				// serialize shader data

				ShaderType type = VertexShader;
				ser.value4b(type);
				ser.text1b(vertex_source, vertex_source.size());
				ser.adapter().flush();

				archive_manager.add_resource("test_data.bin", "debug_vertex_shader.glsl", ResourceData{ buffer });
			}

			// create fragment shader
			{
				std::string fragment_source = fragment_shader_source;
				std::vector<std::byte> buffer(fragment_source.size() + sizeof(ShaderType));
				bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<std::byte>>> ser{ buffer };
				// serialize shader data

				ShaderType type = FragmentShader;
				ser.value4b(type);
				ser.text1b(fragment_source, fragment_source.size());
				ser.adapter().flush();

				archive_manager.add_resource("test_data.bin", "debug_fragment_shader.glsl", ResourceData{ buffer });
			}

			// create scene
			{
				Scene scene(registry);

				Node node = scene.create_node();

				Material material;
				material.vertex_shader_resource_id = "debug_vertex_shader.glsl";
				material.fragment_shader_resource_id = "debug_fragment_shader.glsl";
				node.add_component<Material>(material);

				Mesh mesh = mesh_factory.create(vertices, indices);
				node.add_component<Mesh>(mesh);

				Instances instances;
				const int count = 100000;
				instances.transforms.reserve(count);
				RandomFloatGenerator pos_gen(-50.0f, 50.0f);
				RandomFloatGenerator rot_gen(-1.0f, 1.0f);
				RandomFloatGenerator scale_gen(0.1f, 0.8f);
				for (int i = 0; i < count; ++i) {
					float uniform_scale = scale_gen.generate();
					instances.transforms.push_back(Transform{
						{ pos_gen.generate(), pos_gen.generate(), pos_gen.generate() },
						glm::quat(),
						{ uniform_scale, uniform_scale, uniform_scale }
						});
				}
				instances.update_transforms();
				node.add_component<Instances>(instances);

				std::vector<std::byte> buffer;
				bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<std::byte>>> ser{ buffer };

				ser.object(scene);

				archive_manager.add_resource("test_data.bin", "test_instanced_cube_node", ResourceData{ buffer });

			}
		}

		archive_manager.save();
	}

	void Engine::load_instanced_cubes_test_scene() {
		namespace fs = std::filesystem;

		archive_manager.load();
		const Resource* resource = archive_manager.load_resource("test_instance_cubed_node");

		bitsery::Deserializer<bitsery::InputBufferAdapter<std::byte>> des{ resource->data.bytes };

		des.object(test_scene);
	}

	void Engine::create_test_scene_data_file() {
		Scene scene(registry);

		Material material = material_factory.load_material({
			{ShaderType::VertexShader, vertex_shader_source},
			{ShaderType::FragmentShader, fragment_shader_source}
			});

		Mesh mesh = factory.cube(1.0f);

		Node cube = scene.create_node();
		cube.add_component<Transform>(Transform{ { 0.0f, 0.0f, 0.0f } });
		cube.add_component<Mesh>(mesh);
		cube.add_component<Material>(material);
		cube.add_component<Renderable>(true);

		Instances instances;

		const int count = 100000;

		instances.transforms.reserve(count);

		RandomFloatGenerator pos_gen(-50.0f, 50.0f);
		RandomFloatGenerator rot_gen(-1.0f, 1.0f);
		RandomFloatGenerator scale_gen(0.1f, 0.8f);

		for (int i = 0; i < count; ++i) {
			float uniform_scale = scale_gen.generate();
			instances.transforms.push_back(Transform{
				{ pos_gen.generate(), pos_gen.generate(), pos_gen.generate() },
				glm::quat(),
				{ uniform_scale, uniform_scale, uniform_scale }
				});
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
		if (true) {
			create_instanced_cubes_test_scene();
			//
		}
		else {
			const bool regenerate = !true;

			Material material = material_factory.load_material({
				{ShaderType::VertexShader, vertex_shader_source},
				{ShaderType::FragmentShader, fragment_shader_source}
				});

			Mesh mesh = factory.cube(1.0f);

			resource_manager.add_mesh(mesh);
			resource_manager.add_material(material);

			Scene scene(registry);

			if (regenerate) {
				create_test_scene_data_file();
			}

			load_test_scene_data_file(scene);

			cout << "Scene loaded" << endl;
			cout << "Nodes: " << scene.get_all_nodes().size() << endl;

			for (auto& node : scene.get_all_nodes()) {
				node.add_component<Mesh>(mesh);
				node.add_component<Material>(material);
				node.get_component<Renderable>().value = true;

				cout << "Node " << ((int)node.get_entity_id()) << ": " << node.get_num_components() << " components" << endl;
				cout << "  EnTT valid: " << node.get_registry().valid(node.get_entity_id()) << endl;
				cout << "  Transform: " << node.has_component<Transform>() << endl;
				cout << "  Instances: " << node.has_component<Instances>() << endl;
				cout << "    Instance count: " << node.get_component<Instances>().transforms.size() << endl;
				cout << "    Instances component ptr: " << &node.get_component<Instances>() << endl;
				cout << "  Material: " << node.has_component<Material>() << endl;
				cout << "  Mesh: " << node.has_component<Mesh>() << endl;
				cout << "  Renderable: " << node.has_component<Renderable>() << endl;
				cout << "    value: " << node.get_component<Renderable>().value << endl;
			}
		}

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
