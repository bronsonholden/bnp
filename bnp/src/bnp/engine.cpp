#include <bnp/engine.h>

#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>
#include <bnp/helpers/random_float_generator.hpp>
#include <bnp/helpers/color_helper.hpp>
#include <bnp/serializers/scene.hpp>
#include <bnp/serializers/graphics.hpp>
#include <bnp/managers/archive_manager.h>
#include <bnp/ui/file_browser.h>
#include <bnp/ui/scene_inspector.h>
#include <bnp/ui/node_inspector.h>

#include <string>
#include <fstream>
#include <filesystem>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
using namespace std;

// Basic vertex shader source code
const char* vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
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

const char* instanced_vertex_shader_source = R"(
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

uniform sampler2D sprite_texture;

const float muls[3] = float[](1.0f, 1.1f, 0.9f);

void main() {
    float row = gl_FragCoord.y;
    vec3 light_dir = normalize(vec3(2.0, 1.0, 0.3));
    vec4 tex_color = texture(sprite_texture, tex_coords);
    //float diff = max(dot(normal, light_dir), 0.0);
    //vec3 diffuse = diff * sprite_texture;
    int idx = int(mod(floor(row / 3.0), 3.0));
    tex_color *= muls[idx];
    frag_color = tex_color;
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

		Node node = test_scene.create_node();

		node.add_component<Transform>(Transform{ glm::vec3(0.0f), glm::quat(), glm::vec3(1.0f) });

		//glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);

		const glm::vec4 clear_color = ColorHelper::hex_to_rgba("a3d9ff");
		glClearColor(
			clear_color.r,
			clear_color.g,
			clear_color.b,
			clear_color.a
		);
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
				std::string vertex_source = instanced_vertex_shader_source;
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
				const int count = 50;
				instances.transforms.reserve(count);
				RandomFloatGenerator pos_gen(-10.0f, 10.0f);
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

		const Resource* resource = archive_manager.get_resource("scene_test_instanced_cubes");
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

				registry.patch<Material>(entity, [&loaded_material](auto& material) {
					material.shader_id = loaded_material.shader_id;
					});
			}
		}
	}

	void Engine::run() {
		//create_instanced_cubes_test_scene();
		//load_instanced_cubes_test_scene();

		const float fixed_dt = 1.0f / 60.0f;

		time.start();

		auto context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGui_ImplSDL2_InitForOpenGL(window.get_sdl_window(), window.get_gl_context());
		ImGui_ImplOpenGL3_Init();

		FileBrowser file_browser;
		SceneInspector scene_inspector(test_scene);

		std::filesystem::path root = PROJECT_ROOT;

		{
			MeshFactory mesh_factory;
			Node node = test_scene.create_node();
			Mesh mesh = mesh_factory.box();
			Mesh cube = mesh_factory.cube(1.0f);
			Texture texture = texture_factory.load_from_file(root / "bnp/resources/sprites/out/squirrel.png");
			Material material = material_factory.load_material({
				{ShaderType::VertexShader, vertex_shader_source},
				{ShaderType::FragmentShader, fragment_shader_source}
				});
			texture.resource_id = "squirrel_spritesheet";
			node.add_component<Texture>(texture);
			node.add_component<Mesh>(cube);
			node.add_component<Transform>(Transform{ glm::vec3(-120, -120, 0), glm::quat(),  glm::vec3(240, 240, 1) });
			node.add_component<Renderable>(true);
			node.add_component<Material>(material);
		}

		while (window.open) {

			float width = static_cast<float>(window.get_width());
			float height = static_cast<float>(window.get_height());
			Camera camera({
				glm::vec3(0.0f, 0.0f, 500.0f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::ortho(
					-width / 2,
					width / 2,
					-height / 2,
					height / 2,
					0.1f,
					10000.0f
					)
				});

			SDL_Event event;

			while (window.poll(event)) {
				ImGui_ImplSDL2_ProcessEvent(&event);
			}

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
			render_manager.render(registry, renderer, camera);
			render_manager.render_instances(registry, renderer, camera);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(window.get_sdl_window());
			ImGui::NewFrame();

			file_browser.render();
			scene_inspector.render();

			if (file_browser.has_selection()) {
				std::string path = file_browser.get_selected_path();
				ImGui::Text("Selected path: %s", path.c_str());
			}

			if (scene_inspector.get_inspected_entity()) {
				Node node = test_scene.get_node(*scene_inspector.get_inspected_entity());
				NodeInspector node_inspector(node);
				node_inspector.render();
			}

			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			window.swap();
		}

		resource_manager.cleanup(registry);
	}

	void Engine::update(float dt) {

	}

	void Engine::fixed_update() {

	}
}
