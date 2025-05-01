#include <bnp/engine.h>

#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>
#include <bnp/components/graphics.h>
#include <bnp/components/controllable.h>
#include <bnp/components/global.h>
#include <bnp/helpers/random_float_generator.hpp>
#include <bnp/factories/sprite_factory.h>
#include <bnp/factories/script_factory.h>
#include <bnp/helpers/color_helper.hpp>
#include <bnp/serializers/scene.hpp>
#include <bnp/serializers/graphics.hpp>
#include <bnp/managers/archive_manager.h>
#include <bnp/ui/file_browser.h>
#include <bnp/ui/scene_inspector.h>
#include <bnp/ui/node_inspector.h>
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
		test_scene(registry),
		physics_manager(),
		script_factory(resource_manager, physics_manager),
		behavior_manager(physics_manager)
	{
		registry.emplace<Global>(registry.create(), Global{
			&physics_manager.get_world()
			});
		renderer.initialize();
		archive_manager.load();

		Node node = test_scene.create_node();

		node.add_component<Transform>(Transform{ glm::vec3(0.0f), glm::quat(), glm::vec3(1.0f) });

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
		renderer.shutdown();
	}

	Node Engine::load_sprite(std::filesystem::path sprite_path, std::filesystem::path json_path) {
		Node node = test_scene.create_node();
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
			glm::vec3(2.5, 0, 0),
			glm::quat(),
			glm::vec3(1)
			});
		squirrel.add_component<Identity>(Identity{ "Squirrel" });
		squirrel.add_component<Perception>(Perception{
			{
				{"Bee", 1}
			}
			});

		Node repulsor_field = test_scene.create_node();
		repulsor_field.add_component<Parent>(Parent{ squirrel.get_entity_id() });
		repulsor_field.add_component<Transform>(Transform{ glm::vec3(0) });
		squirrel.add_component<FlowField2D>(FlowField2D{
			0.2f,
			{ 40, 40 },
			{ 0, 0 }
			});

		squirrel.add_component<Motility>(Motility{ 1.8f });
		squirrel.add_component<Controllable>(Controllable{ true });

		{
			Node ground = test_scene.create_node();

			b2BodyDef body_def;
			body_def.position.Set(0.0f, -0.5f);  // Set Y to your desired "floor"
			body_def.type = b2_staticBody;

			// Create a box shape for the ground
			b2PolygonShape box;
			box.SetAsBox(10.0f, 0.5f);  // Width, Height (flat platform)

			b2FixtureDef fixture_def;
			fixture_def.shape = &box;
			fixture_def.friction = 0.0f;

			// Attach the shape to the ground body
			physics_manager.add_body(ground, body_def, fixture_def);

			ground.add_component<Renderable>(true);
		}

		{
			Node ceiling = test_scene.create_node();

			b2BodyDef body_def;
			body_def.position.Set(0.0f, 1.0f);  // Set Y to your desired "floor"
			body_def.type = b2_staticBody;

			// Create a box shape for the ground
			b2PolygonShape box;
			box.SetAsBox(1.5f, 0.15f);  // Width, Height (flat platform)

			b2FixtureDef fixture_def;
			fixture_def.shape = &box;
			fixture_def.friction = 0.0f;

			// Attach the shape to the ground body
			physics_manager.add_body(ceiling, body_def, fixture_def);

			ceiling.add_component<Renderable>(true);
		}

		{
			Node ceiling = test_scene.create_node();

			b2BodyDef body_def;
			body_def.position.Set(4.0f, 0.1f);  // Set Y to your desired "floor"
			body_def.type = b2_staticBody;

			// Create a box shape for the ground
			b2PolygonShape box;
			box.SetAsBox(0.5f, 0.2f);  // Width, Height (flat platform)

			b2FixtureDef fixture_def;
			fixture_def.shape = &box;
			fixture_def.friction = 0.0f;

			// Attach the shape to the ground body
			physics_manager.add_body(ceiling, body_def, fixture_def);

			ceiling.add_component<Renderable>(true);
		}

		{
			Node ceiling = test_scene.create_node();

			b2BodyDef body_def;
			body_def.position.Set(-4.0f, 0.6f);  // Set Y to your desired "floor"
			body_def.type = b2_staticBody;

			// Create a box shape for the ground
			b2PolygonShape box;
			box.SetAsBox(0.5f, 0.2f);  // Width, Height (flat platform)

			b2FixtureDef fixture_def;
			fixture_def.shape = &box;
			fixture_def.friction = 0.0f;

			// Attach the shape to the ground body
			physics_manager.add_body(ceiling, body_def, fixture_def);

			ceiling.add_component<Renderable>(true);
		}

		{
			Node pillar = test_scene.create_node();

			b2BodyDef body_def;
			body_def.position.Set(-2.0f, 4.0f);  // Set Y to your desired "floor"
			body_def.type = b2_staticBody;

			// Create a box shape for the ground
			b2PolygonShape box;
			box.SetAsBox(0.15f, 3.0f);  // Width, Height (flat platform)

			b2FixtureDef fixture_def;
			fixture_def.shape = &box;
			fixture_def.friction = 0.0f;

			// Attach the shape to the ground body
			physics_manager.add_body(pillar, body_def, fixture_def);

			pillar.add_component<Renderable>(true);
		}

		registry.emplace<BeeBehavior>(static_cast<entt::entity>(7), BeeBehavior{});
		//registry.emplace<Identity>(static_cast<entt::entity>(7), Identity{ "Bee" });

		//Node butterfly = load_sprite(
		//	"bnp/sprites/butterfly_red/butterfly_red.png",
		//	"bnp/sprites/butterfly_red/butterfly_red.json"
		//);
		//butterfly.add_component<Transform>(Transform{
		//	glm::vec3(1.75, 0.2, 0),
		//	glm::quat(),
		//	glm::vec3(0.3)
		//	});

		//Node butterfly2 = load_sprite(
		//	"bnp/sprites/butterfly_yellow/butterfly_yellow.png",
		//	"bnp/sprites/butterfly_yellow/butterfly_yellow.json"
		//);
		//butterfly2.add_component<Transform>(Transform{
		//	glm::vec3(0.75, 0.2, 0),
		//	glm::quat(),
		//	glm::vec3(0.3)
		//	});

		//Node grass = load_sprite(
		//	"bnp/sprites/grass_blue_01/grass_blue_01.png",
		//	"bnp/sprites/grass_blue_01/grass_blue_01.json"
		//);
		//grass.add_component<Transform>(Transform{
		//	glm::vec3(2, 0, 0),
		//	glm::quat(),
		//	glm::vec3(1)
		//	});


		//Node grass2 = load_sprite(
		//	"bnp/sprites/grass_blue_01/grass_blue_01.png",
		//	"bnp/sprites/grass_blue_01/grass_blue_01.json"
		//);
		//grass2.add_component<Transform>(Transform{
		//	glm::vec3(2.4, 0, 0),
		//	glm::quat(),
		//	glm::vec3(1)
		//	});

		//Node bush = load_sprite(
		//	"bnp/sprites/bush_blue_01/bush_blue_01.png",
		//	"bnp/sprites/bush_blue_01/bush_blue_01.json"
		//);
		//bush.add_component<Transform>(Transform{
		//	glm::vec3(1, 0, 0),
		//	glm::quat(),
		//	glm::vec3(1)
		//	});

		/*Node arch = load_sprite(
			"bnp/sprites/arch_01/arch_01.png",
			"bnp/sprites/arch_01/arch_01.json"
		);
		arch.add_component<Transform>(Transform{
			glm::vec3(1, 0, 0),
			glm::quat(),
			glm::vec3(4)
			});*/

			// pre-run setup
		physics_manager.generate_sprite_bodies(registry);

		Controller controller(registry, squirrel.get_entity_id());

		std::filesystem::path root = PROJECT_ROOT;
		root = root / "bnp";
		script_factory.load_from_file(squirrel, root / "resources/scripts/log_test.lua");

		while (window.open) {
			float width = static_cast<float>(window.get_width()) / 144;
			float height = static_cast<float>(window.get_height()) / 144;

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

			update(dt);

			window.clear();

			// rendering
			//render_manager.render(registry, renderer, camera);
			//render_manager.render_instances(registry, renderer, camera);
			render_manager.render_sprites(registry, renderer, camera);
			render_manager.render_wireframes(registry, renderer, camera);
			render_manager.render_water2d(registry, renderer, camera);
			//render_manager.render_flow_field_2ds(registry, renderer, camera);

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

			ImGui::Begin("Debug Rendering");
			if (ImGui::Button("Toggle Flow Field Render Reverse")) {
				render_manager.render_flow_field_2d_reverse = !render_manager.render_flow_field_2d_reverse;
			}
			ImGui::End();

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
}
