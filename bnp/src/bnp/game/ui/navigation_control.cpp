#include <bnp/core/logger.hpp>
#include <bnp/game/ui/navigation_control.h>
#include <bnp/game/managers/navigation_manager.h>
#include <bnp/serializers/registry.hpp>

#include <imgui.h>
#include <fstream>

#include <bnp/helpers/filesystem_helper.h>
#include <bnp/game/components/save.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>

namespace bnp {
namespace Game {
namespace UI {

void NavigationControl::render(entt::registry& registry) {
	if (ImGui::Button("Galaxy Map")) {
		Game::Manager::NavigationManager navigation_manager;
		navigation_manager.hide_galaxy_map(registry);
		navigation_manager.hide_celestial_map(registry);
		navigation_manager.hide_system_map(registry);
		navigation_manager.show_galaxy_map(registry);
	}

	if (ImGui::Button("Template Save")) {
		entt::entity test_entity = registry.create();
		registry.emplace<Component::SaveData>(test_entity);
		registry.emplace<Component::FakeDataA>(test_entity, 2);
		registry.emplace<Component::FakeDataB>(test_entity, 3.1415f);

		entt::entity test_entity2 = registry.create();
		registry.emplace<Component::SaveData>(test_entity2);
		registry.emplace<Component::FakeDataA>(test_entity2, 4);
		registry.emplace<Component::FakeDataB>(test_entity2, 6.2830f);

		{
			std::ofstream os(data_dir() / "test_save.bin");
			bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
			bnp::serialize<decltype(ser), Component::SaveData, Component::FakeDataA, Component::FakeDataB>(ser, registry, 1);

			ser.adapter().flush();
			registry.destroy(test_entity);
			registry.destroy(test_entity2);
		}

	}

	if (ImGui::Button("Template load")) {
		{
			std::ifstream is(data_dir() / "test_save.bin");
			bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };
			uint32_t version;
			auto view = bnp::deserialize<decltype(des), Component::SaveData, Component::FakeDataA, Component::FakeDataB>(des, registry, &version);

			Log::info("version %d", version);
			for (auto entity : view) {
				Log::info("FakeDataA: %d", registry.get<Component::FakeDataA>(entity).value);
				Log::info("FakeDataB: %.5f", registry.get<Component::FakeDataB>(entity).value);
			}
		}
	}
}

}
}
}
