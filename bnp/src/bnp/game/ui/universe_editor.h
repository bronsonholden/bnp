#pragma once

#include <bnp/game/components/universe.h>

#include <entt/entt.hpp>
#include <filesystem>

namespace bnp {
namespace Game {
namespace UI {

class UniverseEditor {
public:
	UniverseEditor() = default;

	void initialize(entt::registry& registry);
	void render(entt::registry& registry);

	void save_to_file(entt::registry& registry, std::filesystem::path file_path);

private:
	void render_system_editor(entt::registry& registry, Game::Component::System::ID system_id);
	void render_celestial_editor(entt::registry& registry, Game::Component::Celestial::ID celestial_id);
};

}
}
}
