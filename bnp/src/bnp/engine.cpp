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
#include <bnp/helpers/filesystem_helper.h>
#include <bnp/game/ui/core_state_inspector.h>

#include <bnp/behaviors/bee_behavior_planner.h>

#include <bnp/game/prefabs/squirrel.h>
#include <bnp/game/prefabs/celestials.h>
#include <bnp/game/prefabs/ui.h>
#include <bnp/game/prefabs/galaxy.h>
#include <bnp/game/components/fleet.h>
#include <bnp/game/ui/navigation_control.h>

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

namespace bnp {

Engine::Engine()
	: archive_manager(std::filesystem::path(PROJECT_ROOT) / "bnp/data"),
	physics_manager(registry),
	script_factory(resource_manager, physics_manager),
	behavior_manager(),
	world_2d_manager(),
	file_browser(),
	scene_inspector(registry)
{
	registry.emplace<Global>(registry.create(), Global{
		&physics_manager.get_world()
		});

	renderer.initialize(window.get_width(), window.get_height());

	archive_manager.load();

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
	render_manager.circle_mesh = MeshFactory().circle();

	ResourceManager::singleton = &resource_manager;
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

	Log::info_wide(L"Data directory: %s", data_dir().c_str());
	Log::info_wide(L"Save directory: %s", save_dir().c_str());

	time.start();

	auto context = ImGui::CreateContext();
	ImGui::SetCurrentContext(context);
	ImGui_ImplSDL2_InitForOpenGL(window.get_sdl_window(), window.get_gl_context());
	ImGui_ImplOpenGL3_Init();

	// game-specific code, need to move
	Game::Prefab::Galaxy::model(registry);
	navigation_manager.show_galaxy_map(registry);

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

		render();
	}
}

void Engine::update(float dt) {
	// game-specific managers
	navigation_manager.update(registry, dt);
	universe_manager.update(registry, dt);

	// manager updates
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

void Engine::render() {
	/*auto& rig = galaxy.get_component<Camera2DRig>();
	glm::vec2 camera_position = rig.camera_worldspace_position;*/
	glm::vec2 camera_position(0);
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

	renderer.front_fb.bind();
	renderer.front_fb.clear();

	renderer.upscale_fb.bind();
	renderer.upscale_fb.clear();
	render_manager.render_planet_2ds(registry, renderer, camera);
	renderer.upscale_fb.unbind();
	renderer.front_fb.bind();
	render_manager.render_fullscreen_quad(renderer, renderer.upscale_fb);

	renderer.upscale_fb.bind();
	renderer.upscale_fb.clear();
	render_manager.render_galaxy_2ds(registry, renderer, camera);
	renderer.upscale_fb.unbind();
	renderer.front_fb.bind();
	render_manager.render_fullscreen_quad(renderer, renderer.upscale_fb);

	renderer.upscale_fb.bind();
	renderer.upscale_fb.clear();
	render_manager.render_primitives(registry, renderer, camera);
	renderer.upscale_fb.unbind();
	renderer.front_fb.bind();
	render_manager.render_fullscreen_quad(renderer, renderer.upscale_fb);
	renderer.front_fb.unbind();

	renderer.front_fb.bind();
	render_manager.render_sprites(registry, renderer, camera);
	renderer.front_fb.unbind();

	render_manager.render_fullscreen_quad(renderer, renderer.front_fb);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window.get_sdl_window());
	ImGui::NewFrame();

	//file_browser.render();
	scene_inspector.render();

	CoreStateInspector().render(registry);
	Game::NavigationControl().render(registry);

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
	}
}
}
