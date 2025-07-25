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
#include <bnp/managers/water_2d_manager.h>
#include <bnp/managers/behavior_manager.h>
#include <bnp/managers/world_2d_manager.h>
#include <bnp/managers/camera_manager.h>
#include <bnp/managers/controller_manager.h>
#include <bnp/managers/ui_manager.h>

// editor UI stuff
#include <bnp/ui/file_browser.h>
#include <bnp/ui/scene_inspector.h>
#include <bnp/ui/node_inspector.h>
#include <bnp/ui/render_manager_inspector.h>
#include <bnp/game/ui/editor_tools.h>

#include <bnp/game/managers/game_manager.h>
#include <bnp/game/managers/navigation_manager.h>
#include <bnp/game/managers/universe_manager.h>

#include <entt/entt.hpp>

namespace bnp {

class Engine {
public:
	Engine();
	~Engine();

	void run();

protected:
	entt::registry registry;
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
	Water2DManager water_2d_manager;
	BehaviorManager behavior_manager;
	World2DManager world_2d_manager;
	CameraManager camera_manager;
	ControllerManager controller_manager;
	UIManager ui_manager;

	Game::Manager::GameManager game_manager;
	Game::Manager::NavigationManager navigation_manager;
	Game::Manager::UniverseManager universe_manager;

	// editor stuff
	FileBrowser file_browser;
	SceneInspector scene_inspector;
	Game::UI::EditorTools g_editor_tools;

	Camera camera;

	void update(float dt);
	void fixed_update();
	void render();

	void handle_window_event(SDL_Event& event);
	void handle_mouse_button_event(SDL_Event& event);
};

}
