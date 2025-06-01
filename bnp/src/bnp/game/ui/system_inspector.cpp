#include <bnp/game/ui/system_inspector.h>
#include <bnp/game/components/universe.h>

namespace bnp {
namespace Game {

SystemInspector::SystemInspector(entt::registry& _registry)
	: registry(_registry),
	system_entity(entt::null)
{
}

void SystemInspector::render() {
	ImGui::Begin("System Inspector");

	if (system_entity == entt::null) return;

	auto& system = registry.get<Game::Component::System>(system_entity);

	ImGui::Text("System ID: %u", static_cast<uint32_t>(system.id));
}

}
}
