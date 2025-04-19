#pragma once

#include <bnp/core/window.h>
#include <bnp/core/time_system.h>
#include <bnp/core/scene.h>
#include <bnp/graphics/renderer.h>
#include <bnp/factories/material_factory.h>
#include <bnp/factories/mesh_factory.h>
#include <bnp/factories/texture_factory.h>
#include <bnp/factories/script_factory.h>
#include <bnp/managers/resource_manager.h>
#include <bnp/managers/render_manager.h>
#include <bnp/managers/archive_manager.h>
#include <bnp/managers/sprite_animation_manager.h>
#include <bnp/managers/physics_manager.h>
#include <bnp/managers/motility_manager.h>
#include <bnp/managers/hierarchy_manager.h>
#include <bnp/managers/water2d_manager.h>

#include <entt/entt.hpp>

namespace bnp {
	class Engine {
	public:
		Engine();
		~Engine();

		void run();

	protected:
		entt::registry registry;
		Scene test_scene;
		Window window;
		TimeSystem time;
		Renderer renderer;

		MeshFactory factory;
		MaterialFactory material_factory;
		TextureFactory texture_factory;
		ScriptFactory script_factory;

		ArchiveManager archive_manager;
		RenderManager render_manager;
		ResourceManager resource_manager;
		SpriteAnimationManager sprite_animation_manager;
		PhysicsManager physics_manager;
		MotilityManager motility_manager;
		HierarchyManager hierarchy_manager;
		Water2DManager water2d_manager;

		void update(float dt);
		void fixed_update();

		Node load_sprite(std::filesystem::path sprite_path, std::filesystem::path json_path);
	};
}
