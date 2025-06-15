#pragma once

#include <entt/entt.hpp>
#include <filesystem>

namespace bnp {
namespace Game {
namespace UI {

class BlueprintsEditor {
public:
	BlueprintsEditor() = default;

	void initialize(entt::registry& registry);
	void render(entt::registry& registry);

private:
	entt::entity edit_ship_blueprint_entity = entt::null;
	entt::entity edit_engine_blueprint_entity = entt::null;
	entt::entity edit_fluid_storage_blueprint_entity = entt::null;
	void render_ship_blueprints_section(entt::registry& registry);
	void render_engine_blueprints_section(entt::registry& registry);
	void render_fluid_storage_blueprints_section(entt::registry& registry);
	void render_edit_ship_blueprint_section(entt::registry& registry);
	void render_edit_engine_blueprint_section(entt::registry& registry);
	void render_edit_fluid_storage_blueprint_section(entt::registry& registry);
	void save_to_file(entt::registry& registry, std::filesystem::path file_path);
};

}
}
}
