#include <bnp/engine.h>

#include <bnp/core/node.hpp>
#include <bnp/core/logger.hpp>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>
#include <bnp/components/controllable.h>
#include <bnp/components/global.h>
#include <bnp/components/world.h>
#include <bnp/helpers/random_float_generator.hpp>
#include <bnp/factories/sprite_factory.h>
#include <bnp/factories/script_factory.h>
#include <bnp/helpers/color_helper.hpp>
#include <bnp/helpers/camera_helper.h>
#include <bnp/helpers/ui_helper.h>
#include <bnp/serializers/scene.hpp>
#include <bnp/serializers/graphics.hpp>
#include <bnp/managers/archive_manager.h>
#include <bnp/ui/file_browser.h>
#include <bnp/ui/scene_inspector.h>
#include <bnp/ui/node_inspector.h>
#include <bnp/ui/render_manager_inspector.h>

#include <bnp/behaviors/bee_behavior_planner.h>

#include <bnp/game/prefabs/squirrel.h>
#include <bnp/game/prefabs/celestials.h>
#include <bnp/game/prefabs/ui.h>

#include <vector>
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
		physics_manager(registry),
		script_factory(resource_manager, physics_manager),
		behavior_manager(),
		world_2d_manager()
	{
		registry.emplace<Global>(registry.create(), Global{
			&physics_manager.get_world()
			});

		renderer.initialize(window.get_width(), window.get_height());

		archive_manager.load();

		//glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);

		//const glm::vec4 clear_color = ColorHelper::hex_to_rgba("a3d9ff");
		const glm::vec4 clear_color = glm::vec4(0, 0, 0, 1);
		glClearColor(
			clear_color.r,
			clear_color.g,
			clear_color.b,
			clear_color.a
		);

		render_manager.wireframe_material = material_factory.load_material_from_files({
			{ ShaderType::VertexShader, "resources/shaders/wireframe_vertex_shader.glsl" },
			{ ShaderType::FragmentShader, "resources/shaders/wireframe_fragment_shader.glsl" }
			});

		render_manager.physics_body_2d_material = material_factory.load_material_from_files({
			{ ShaderType::VertexShader, "resources/shaders/physics_body_2d_vertex_shader.glsl" },
			{ ShaderType::FragmentShader, "resources/shaders/physics_body_2d_fragment_shader.glsl" }
			});

		render_manager.quad_material = material_factory.load_material_from_files({
			{ ShaderType::VertexShader, "resources/shaders/quad_vertex_shader.glsl" },
			{ ShaderType::FragmentShader, "resources/shaders/quad_fragment_shader.glsl" }
			});

		render_manager.sprite_mesh = MeshFactory().box();
		render_manager.line_mesh = MeshFactory().line();
	}

	Engine::~Engine() {
		render_manager.wireframe_material.cleanup();
		render_manager.physics_body_2d_material.cleanup();
		render_manager.quad_material.cleanup();
		render_manager.sprite_mesh.cleanup();
		render_manager.line_mesh.cleanup();

		registry.clear();
		resource_manager.cleanup();
		renderer.shutdown();
	}

	void Engine::run() {
		const float fixed_dt = 1.0f / 60.0f;

		time.start();

		auto context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGui_ImplSDL2_InitForOpenGL(window.get_sdl_window(), window.get_gl_context());
		ImGui_ImplOpenGL3_Init();

		FileBrowser file_browser;
		SceneInspector scene_inspector(registry);

		Node galaxy = Prefab::Celestials::galaxy(registry, resource_manager);

		Node dot = Prefab::Celestials::galaxy_map_dot(registry, resource_manager);

		//Node play_button = Prefab::UI::play_button(registry, resource_manager);

		//squirrel.get_component<PhysicsBody2D>().body->SetTransform(b2Vec2(2.0f, 1.5f), 0);

		std::filesystem::path root = PROJECT_ROOT;
		root = root / "bnp";
		//script_factory.load_from_file(squirrel, root / "resources/scripts/log_test.lua");

		while (window.open) {
			SDL_Event event;

			while (window.poll(event)) {
				switch (event.type) {
				case SDL_QUIT:
					window.open = false;
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					handle_mouse_button_event(event);
					break;
				case SDL_WINDOWEVENT:
					handle_window_event(event);
					break;
				default:
					;
				}

				ImGui_ImplSDL2_ProcessEvent(&event);
				controller_manager.process_event(registry, event);
			}

			time.new_frame();

			float dt = time.delta_time();

			controller_manager.update_keyboard_input_observers(registry);

			while (time.needs_fixed_update()) {
				fixed_update();
				time.consume_fixed_step();
			}

			update(dt);

			int x, y;
			window.get_mouse_position(&x, &y);
			glm::vec4 mouse_worldspace_pos = ui_space_to_world_space(
				camera.perspective,
				static_cast<float>(window.get_width()),
				static_cast<float>(window.get_height()),
				static_cast<float>(x),
				static_cast<float>(y)
			);
			ui_manager.update_mouse_position(
				registry,
				mouse_worldspace_pos
			);

			auto& rig = galaxy.get_component<Camera2DRig>();
			glm::vec2 camera_position = rig.camera_worldspace_position;
			glm::vec2 pixel_space = glm::floor(camera_position * 64.0f);
			glm::vec2 snap_position = pixel_space / 64.0f;

			camera = Camera{
				glm::vec3(snap_position, 0.0f),
				glm::vec3(snap_position, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::mat4(1.0)
			};

			configure_pixel_perfect_camera(camera, window.get_width(), window.get_height());

			window.clear();

			// rendering

			renderer.obstacle_fb.bind();
			renderer.obstacle_fb.clear();
			render_manager.render_physics_body_2ds(registry, renderer, camera);
			renderer.obstacle_fb.unbind();

			renderer.upscale_fb.bind();
			renderer.upscale_fb.clear();
			render_manager.render_planet_2ds(registry, renderer, camera);
			render_manager.render_galaxy_2ds(registry, renderer, camera);
			render_manager.render_primitives(registry, renderer, camera);
			renderer.upscale_fb.unbind();

			renderer.front_fb.bind();
			renderer.front_fb.clear();
			render_manager.render_fullscreen_quad(renderer, renderer.upscale_fb);
			render_manager.render_sprites(registry, renderer, camera);
			renderer.front_fb.unbind();

			render_manager.render_fullscreen_quad(renderer, renderer.front_fb);

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
				Node node(registry, *scene_inspector.get_inspected_entity());
				NodeInspector node_inspector(node);
				node_inspector.render();
			}

			RenderManagerInspector render_manager_inspector(render_manager);
			render_manager_inspector.render();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			window.swap();
		}
	}

	void Engine::update(float dt) {
		//cout << "fps: " << std::to_string(1.0f / dt) << endl;

		// manager updates
		planet_2d_manager.update(registry, dt);
		controller_manager.update(registry, dt);
		sprite_animation_manager.update(registry, dt);
		motility_manager.update(registry, dt);
		world_2d_manager.update(registry, dt);
		camera_manager.update(registry, dt);
		ui_manager.update(registry, dt);

		// only apply transforms after all game updates have completed
		// so we have the most correct transforms
		hierarchy_manager.update(registry);

		// update behaviors with a clean transform hierarchy
		behavior_manager.update(registry, dt);
		{
			BeeBehaviorPlanner bbp;
			bbp.update(registry, dt);
		}
	}

	void Engine::fixed_update() {
		water_2d_manager.update(registry, time.fixed_delta_time());
		physics_manager.update(registry, time.fixed_delta_time());
	}

	void Engine::handle_window_event(SDL_Event& event) {
		int width = event.window.data1;
		int height = event.window.data2;

		switch (event.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			renderer.resize(width, height);
			break;
		default:
			;
		}
	}

	void Engine::handle_mouse_button_event(SDL_Event& event) {
		if (event.type != SDL_MOUSEBUTTONDOWN) return;

		int screen_width = window.get_width();
		int screen_height = window.get_height();

		if (event.button.button == SDL_BUTTON_LEFT) {
			glm::vec4 click_world_pos = ui_space_to_world_space(
				camera.perspective,
				static_cast<float>(screen_width),
				static_cast<float>(screen_height),
				static_cast<float>(event.button.x),
				static_cast<float>(event.button.y)
			);

			ui_manager.process_click(registry, click_world_pos);

			Log::info("Clicked %.2f, %.2f", click_world_pos.x, click_world_pos.y);
		}
	}
}
