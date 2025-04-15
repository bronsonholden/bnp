#include <bnp/engine.h>

#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>
#include <bnp/components/controllable.h>
#include <bnp/helpers/random_float_generator.hpp>
#include <bnp/graphics/sprite_factory.h>
#include <bnp/helpers/color_helper.hpp>
#include <bnp/serializers/scene.hpp>
#include <bnp/serializers/graphics.hpp>
#include <bnp/managers/archive_manager.h>
#include <bnp/ui/file_browser.h>
#include <bnp/ui/scene_inspector.h>
#include <bnp/ui/node_inspector.h>
#include <bnp/controllers/controller.h>

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
uniform vec2 uv0;
uniform vec2 uv1;

const float adds[3] = float[](0.0f, 0.07f, -0.09f);

void main() {
    float row = gl_FragCoord.y;
    vec3 light_dir = normalize(vec3(2.0, 1.0, 0.3));
    vec4 tex_color = texture(sprite_texture, mix(uv0, uv1, tex_coords));
    //float diff = max(dot(normal, light_dir), 0.0);
    //vec3 diffuse = diff * sprite_texture;
    int idx = int(mod(floor(row / 2.0), 3.0));
    tex_color += vec4(vec3(adds[idx]), tex_color.a);
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

	Node Engine::load_sprite(std::filesystem::path sprite_path, std::filesystem::path json_path) {
		std::filesystem::path root = PROJECT_ROOT;
		Node node = test_scene.create_node();
		Texture texture = texture_factory.load_from_file(root / sprite_path);
		Material material = material_factory.load_material({
			{ShaderType::VertexShader, vertex_shader_source},
			{ShaderType::FragmentShader, fragment_shader_source}
			});
		texture.resource_id = sprite_path.string();
		node.add_component<Texture>(texture);
		node.add_component<Renderable>(true);
		node.add_component<Material>(material);
		resource_manager.add_texture(sprite_path.string(), texture);

		SpriteFactory sprite_factory;

		if (json_path != "") {
			sprite_factory.load_from_aseprite(node, (root / json_path).string());
		}

		return node;
	}

	void Engine::run() {
		const float fixed_dt = 1.0f / 60.0f;

		time.start();

		auto context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGui_ImplSDL2_InitForOpenGL(window.get_sdl_window(), window.get_gl_context());
		ImGui_ImplOpenGL3_Init();

		FileBrowser file_browser;
		SceneInspector scene_inspector(test_scene);


		Node squirrel = load_sprite(
			"bnp/resources/sprites/squirrel/squirrel.png",
			"bnp/resources/sprites/squirrel/squirrel.json"
		);
		squirrel.add_component<Transform>(Transform{
			glm::vec3(0, 0, 0),
			glm::quat(),
			glm::vec3(1)
			});

		squirrel.add_component<Motility>(Motility{ 2.5 });
		squirrel.add_component<Controllable>(Controllable{ true });


		Node grass = load_sprite(
			"bnp/resources/sprites/grass_01/grass_01.png",
			"bnp/resources/sprites/grass_01/grass_01.json"
		);
		grass.add_component<Transform>(Transform{
			glm::vec3(2, 0, 0),
			glm::quat(),
			glm::vec3(1)
			});

		Node bush = load_sprite(
			"bnp/resources/sprites/bush_01/bush_01.png",
			"bnp/resources/sprites/bush_01/bush_01.json"
		);
		bush.add_component<Transform>(Transform{
			glm::vec3(1, 0, 0),
			glm::quat(),
			glm::vec3(1)
			});

		// pre-run setup
		physics_manager.generate_sprite_bodies(registry);

		Controller controller(registry, squirrel.get_entity_id());

		while (window.open) {

			float width = static_cast<float>(window.get_width()) / 192;
			float height = static_cast<float>(window.get_height()) / 192;
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
				controller.process_event(event);
			}

			time.new_frame();

			float dt = time.delta_time();

			while (time.needs_fixed_update()) {
				fixed_update();
				time.consume_fixed_step();
			}

			//cout << "fps: " << std::to_string(1.0f / dt) << endl;

			// manager updates
			sprite_animation_manager.update(registry, dt);
			motility_manager.update(registry, dt);

			// only apply transforms after all game updates have completed
			// so we have the most correct transforms
			hierarchy_manager.update(registry);

			window.clear();

			// rendering
			//render_manager.render(registry, renderer, camera);
			//render_manager.render_instances(registry, renderer, camera);
			render_manager.render_sprites(registry, renderer, camera);
			render_manager.render_wireframes(registry, renderer, camera);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(window.get_sdl_window());
			ImGui::NewFrame();

			//file_browser.render();
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
		physics_manager.update(registry, time.fixed_delta_time());
	}
}
