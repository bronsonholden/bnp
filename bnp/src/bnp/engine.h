#pragma once

#include <bnp/core/window.h>
#include <bnp/core/time_system.h>
#include <bnp/core/scene.h>
#include <bnp/graphics/renderer.h>
#include <bnp/graphics/material_factory.h>
#include <bnp/graphics/mesh_factory.h>
#include <bnp/managers/resource_manager.h>
#include <bnp/managers/render_manager.h>
#include <bnp/managers/archive_manager.h>

#include <entt/entt.hpp>

namespace bnp {
	class Engine {
	public:
		Engine();
		~Engine();

		void run();

	protected:
		// archive test
		void create_instanced_cubes_test_scene();
		void load_instanced_cubes_test_scene();
		Scene test_scene;

		// static files test
		void create_test_scene_data_file();
		void load_test_scene_data_file(Scene& scene);

		entt::registry registry;
		Window window;
		TimeSystem time;
		Renderer renderer;

		MeshFactory factory;
		MaterialFactory material_factory;

		ArchiveManager archive_manager;
		RenderManager render_manager;
		ResourceManager resource_manager;

		void update(float dt);
		void fixed_update();
	};
}
