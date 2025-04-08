#include <bnp/engine.h>

#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>
#include <bnp/helpers/random_float_generator.hpp>
#include <bnp/serializers/scene.hpp>
#include <bnp/serializers/graphics.hpp>
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

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

namespace bnp {

	Engine::Engine()
		: archive_manager(std::filesystem::path(PROJECT_ROOT) / "bnp/data"),
		test_scene(registry)
	{
		renderer.initialize();
		archive_manager.load();

		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
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

			// create mesh data and write
			{
				MeshData mesh_data;
				mesh_factory.create_cube(mesh_data.vertices, mesh_data.indices, 1.0f);

				std::vector<std::byte> buffer;

				bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<std::byte>>> ser{ buffer };
				ser.object(mesh_data);
				ser.adapter().flush();

				cout << "wrote mesh: " << buffer.size() << " bytes" << endl;
				cout << mesh_data.vertices.size() << " vertices" << endl;
				cout << mesh_data.indices.size() << " indices" << endl;

				archive_manager.add_resource("test_data.bin", "test_cube_mesh", ResourceData{ buffer });

			}

			// create vertex shader
			{
				std::string vertex_source = vertex_shader_source;
				std::vector<std::byte> buffer;
				bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<std::byte>>> ser{ buffer };
				// serialize shader data

				ShaderData shader_data{ VertexShader , vertex_source };
				ser.object(shader_data);
				ser.adapter().flush();

				archive_manager.add_resource("test_data.bin", "debug_vertex_shader.glsl", ResourceData{ buffer });
			}

			// create fragment shader
			{
				std::string fragment_source = fragment_shader_source;
				std::vector<std::byte> buffer;
				bitsery::Serializer<bitsery::OutputBufferAdapter<std::vector<std::byte>>> ser{ buffer };
				// serialize shader data

				ShaderData shader_data{ FragmentShader , fragment_source };
				ser.object(shader_data);
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

				Mesh mesh;
				mesh.resource_id = "test_cube_mesh";
				node.add_component<Mesh>(mesh);

				node.add_component<Renderable>(true);

				Instances instances;
				const int count = 50000;
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
				ser.adapter().flush();
				cout << "saved scene_test_instanced_cubes: " << buffer.size() << " bytes" << endl;

				archive_manager.add_resource("test_data.bin", "scene_test_instanced_cubes", ResourceData{ buffer });

			}

			archive_manager.save();
		}

	}

	void Engine::load_instanced_cubes_test_scene() {
		namespace fs = std::filesystem;

		archive_manager.load();

		cout << "scene_test_instanced_cubes: " << archive_manager.get_resource("scene_test_instanced_cubes") << endl;
		cout << "test_cube_mesh: " << archive_manager.get_resource("test_cube_mesh") << endl;
		cout << "debug_vertex_shader.glsl: " << archive_manager.get_resource("debug_vertex_shader.glsl") << endl;
		cout << "debug_fragment_shader.glsl: " << archive_manager.get_resource("debug_fragment_shader.glsl") << endl;

		cout << "has test_data.bin: " << archive_manager.has_archive("test_data.bin") << endl;
		cout << "has test_cube_mesh: " << archive_manager.has_resource("test_cube_mesh") << endl;

		const Resource* resource = archive_manager.get_resource("scene_test_instanced_cubes");
		cout << "loaded scene_test_instanced_cubes: " << resource->data.bytes.size() << " bytes" << endl;
		bitsery::Deserializer<bitsery::InputBufferAdapter<std::vector<std::byte>>> des{ resource->data.bytes.begin(), resource->data.bytes.end() };

		des.object(test_scene);

		for (auto& [entity, node] : test_scene.get_nodes()) {
			cout << "Node " << ((int)node.get_entity_id()) << ": " << node.get_num_components() << " components" << endl;
			cout << "  EnTT valid: " << registry.valid(node.get_entity_id()) << endl;
			cout << "  Transform: " << node.has_component<Transform>() << endl;
			cout << "  Instances: " << node.has_component<Instances>() << endl;
			cout << "    Instance count: " << node.get_component<Instances>().transforms.size() << endl;
			cout << "    Instances component ptr: " << &node.get_component<Instances>() << endl;
			cout << "  Material: " << node.has_component<Material>() << endl;
			cout << "  Mesh: " << node.has_component<Mesh>() << endl;
			cout << "    resource_id: " << node.get_component<Mesh>().resource_id << endl;
			cout << "  Renderable: " << node.has_component<Renderable>() << endl;
			cout << "    value: " << node.get_component<Renderable>().value << endl;
		}

		{
			auto view = registry.view<Mesh, Material>();

			MeshFactory mesh_factory;

			for (auto& entity : view) {
				auto& mesh = registry.get<Mesh>(entity);

				cout << mesh.resource_id << " loading from archive" << endl;

				ResourceIdentifier resource_id = mesh.resource_id;
				MeshData mesh_data;

				const Resource* mesh_resource = archive_manager.get_resource(resource_id);
				cout << "resource ptr: " << mesh_resource << endl;
				const std::vector<std::byte>& buffer = mesh_resource->data.bytes;

				bitsery::Deserializer<bitsery::InputBufferAdapter<std::vector<std::byte>>> des{ buffer.begin(), buffer.end() };
				des.object(mesh_data);

				// todo: handle already-loaded mesh (e.g. one mesh used on two entities)
				Mesh loaded_mesh = mesh_factory.create(mesh_data.vertices, mesh_data.indices);

				registry.patch<Mesh>(entity, [&loaded_mesh](auto& mesh) {
					cout << "mesh.va_id: " << loaded_mesh.va_id << endl;
					cout << "mesh.vb_id: " << loaded_mesh.vb_id << endl;
					cout << "mesh.eb_id: " << loaded_mesh.eb_id << endl;

					mesh.va_id = loaded_mesh.va_id;
					mesh.vb_id = loaded_mesh.vb_id;
					mesh.eb_id = loaded_mesh.eb_id;

					mesh.vertex_count = loaded_mesh.vertex_count;
					});
			}
		}

		{
			auto view = registry.view<Instances>();

			for (auto& entity : view) {
				auto& instances = registry.get<Instances>(entity);

				instances.update_transforms();
			}
		}

		{
			auto view = registry.view<Material>();

			MaterialFactory material_factory;

			for (auto& entity : view) {
				auto& material = registry.get<Material>(entity);
				std::vector<std::pair<ShaderType, std::string>> shaders;

				if (material.vertex_shader_resource_id.size()) {
					ShaderData shader_data;
					const Resource* resource = archive_manager.get_resource(material.vertex_shader_resource_id);
					const std::vector<std::byte>& bytes = resource->data.bytes;
					bitsery::Deserializer<bitsery::InputBufferAdapter<std::vector<std::byte>>> des{ bytes.begin(), bytes.end() };
					des.object(shader_data);
					shaders.push_back({ shader_data.shader_type, shader_data.source });
				}
				if (material.fragment_shader_resource_id.size()) {
					ShaderData shader_data;
					const Resource* resource = archive_manager.get_resource(material.fragment_shader_resource_id);
					const std::vector<std::byte>& bytes = resource->data.bytes;
					bitsery::Deserializer<bitsery::InputBufferAdapter<std::vector<std::byte>>> des{ bytes.begin(), bytes.end() };
					des.object(shader_data);
					shaders.push_back({ shader_data.shader_type, shader_data.source });
				}

				// todo: handle already-loaded material (e.g. one material used on two entities)
				Material loaded_material = material_factory.load_material(shaders);

				// issue here?
				registry.patch<Material>(entity, [&loaded_material](auto& material) {
					material.shader_id = loaded_material.shader_id;
					});
			}
		}
	}

	void Engine::run() {
		create_instanced_cubes_test_scene();
		load_instanced_cubes_test_scene();

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
