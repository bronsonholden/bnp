#include <bnp/engine.h>

#include <bnp/core/node.hpp>
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
#include <bnp/serializers/scene.hpp>
#include <bnp/serializers/graphics.hpp>
#include <bnp/managers/archive_manager.h>
#include <bnp/ui/file_browser.h>
#include <bnp/ui/scene_inspector.h>
#include <bnp/ui/node_inspector.h>
#include <bnp/ui/render_manager_inspector.h>
#include <bnp/controllers/controller.h>

#include <bnp/behaviors/bee_behavior_planner.h>

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
		world2d_manager()
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
	}

	Engine::~Engine() {
		registry.clear();
		renderer.shutdown();
	}

	Node Engine::load_sprite(std::filesystem::path sprite_path, std::filesystem::path json_path) {
		Node node(registry);
		Texture texture = resource_manager.load_texture(sprite_path.string(), sprite_path);
		Material material = resource_manager.load_material("sprite_material", {
			{ShaderType::VertexShader, "resources/shaders/sprite_vertex_shader.glsl"},
			{ShaderType::FragmentShader, "resources/shaders/sprite_fragment_shader.glsl"}
			});
		node.add_component<Texture>(texture);
		node.add_component<Renderable>(true);
		node.add_component<Material>(material);

		SpriteFactory sprite_factory;
		sprite_factory.load_from_aseprite(node, json_path);

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
		SceneInspector scene_inspector(registry);

		Node squirrel = load_sprite(
			"resources/sprites/squirrel/squirrel.png",
			"resources/sprites/squirrel/squirrel.json"
		);
		squirrel.add_component<Transform>(Transform{
			glm::vec3(3, 3, 0),
			glm::quat(),
			glm::vec3(1)
			});
		squirrel.add_component<Identity>(Identity{ "Squirrel" });
		squirrel.add_component<Perception>(Perception{
			{
				{"Bee", 1}
			}
			});

		squirrel.add_component<FlowField2D>(FlowField2D{
			0.25f,
			{ 40, 40 },
			{ 0, 0 }
			});

		squirrel.add_component<Motility>(Motility{ 2.5f });
		squirrel.add_component<Controllable>(Controllable{ true });

		squirrel.add_component<Camera2DRig>(Camera2DRig{
			{ 0.0f, 1.0f }
			});

		// pre-run setup
		physics_manager.generate_sprite_bodies(registry);

		Controller controller(registry, squirrel.get_entity_id());

		std::filesystem::path root = PROJECT_ROOT;
		root = root / "bnp";
		script_factory.load_from_file(squirrel, root / "resources/scripts/log_test.lua");

		{
			Tilemap tilemap;
			tilemap.tiles = {
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
				0,0,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
			};
			tilemap.height = 15;
			tilemap.width = tilemap.tiles.size() / tilemap.height;

			std::vector<glm::ivec4> rects = tilemap.decompose();

			entt::entity world = registry.create();

			auto& world2d = registry.emplace<World2D>(world, World2D{
				0.25f,
				{ 10, 10 },
				{ 5.0f, 2.5f }
				});
			registry.emplace<Tilemap>(world, tilemap);

			glm::ivec2 partition = world2d.get_active_partition();
		}

		auto& transform = squirrel.get_component<Transform>();

		glm::vec2 camera_position = transform.world_transform[3];

		while (window.open) {

			SDL_Event event;

			while (window.poll(event)) {
				switch (event.type) {
				case SDL_QUIT:
					window.open = false;
					break;
				case SDL_WINDOWEVENT:
					handle_window_event(event);
					break;
				default:
					;
				}

				ImGui_ImplSDL2_ProcessEvent(&event);
				controller.process_event(event);
			}

			time.new_frame();

			float dt = time.delta_time();

			while (time.needs_fixed_update()) {
				fixed_update();
				time.consume_fixed_step();
			}

			update(dt);

			auto& rig = squirrel.get_component<Camera2DRig>();
			glm::vec2 camera_position = rig.camera_worldspace_position;
			glm::vec2 pixel_space = glm::floor(camera_position * 64.0f);
			glm::vec2 snap_position = pixel_space / 64.0f;

			Camera camera({
				glm::vec3(snap_position, 0.0f),
				glm::vec3(snap_position, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::mat4(1.0)
				});

			configure_pixel_perfect_camera(camera, window.get_width(), window.get_height());

			window.clear();

			// rendering

			renderer.obstacle_fb.bind();
			renderer.obstacle_fb.clear();
			render_manager.render_physics_body_2ds(registry, renderer, camera);
			renderer.obstacle_fb.unbind();

			renderer.upscale_fb.bind();
			renderer.upscale_fb.clear();
			render_manager.render_bezier_sprites(registry, renderer, camera);
			renderer.upscale_fb.unbind();

			//render_manager.render(registry, renderer, camera);
			//render_manager.render_instances(registry, renderer, camera);
			renderer.front_fb.bind();
			renderer.front_fb.clear();
			render_manager.render_sprites(registry, renderer, camera);
			render_manager.render_water2d(registry, renderer, camera);
			render_manager.render_wireframes(registry, renderer, camera);
			render_manager.render_fullscreen_quad(renderer, renderer.upscale_fb);
			//render_manager.render_flow_field_2ds(registry, renderer, camera);
			renderer.front_fb.unbind();

			glViewport(0, 0, window.get_width(), window.get_height());
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
		sprite_animation_manager.update(registry, dt);
		motility_manager.update(registry, dt);
		world2d_manager.update(registry, dt);
		camera_manager.update(registry, dt);

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
		water2d_manager.update(registry, time.fixed_delta_time());
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
}
