#include <bnp/helpers/filesystem_helper.h>
#include <bnp/game/ui/items_editor.h>
#include <bnp/game/components/inventory.h>
#include <bnp/game/serializers/inventory.hpp>

#include <imgui.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <filesystem>
#include <fstream>

namespace bnp {
namespace Game {
namespace UI {

void ItemsEditor::render(entt::registry& registry) {
	ImGui::Begin("Items editor");

	if (!loaded) {
		load_from_file(registry);
		loaded = true;
	}

	if (ImGui::Button("New item")) {
		// todo: get next ID from index?
		Game::Component::Item::ID next_id = 1;
		auto items = registry.view<Game::Component::Item>();
		for (auto entity : items) {
			auto& item = items.get<Game::Component::Item>(entity);
			if (item.id >= next_id) next_id = item.id + 1;
		}

		registry.emplace<Game::Component::Item>(registry.create(), Game::Component::Item{
			.id = next_id,
			.name = "<New item>"
			});
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

	if (ImGui::Button("Save")) {
		save_to_file(registry);
	}

	ImGui::Separator();

	std::vector<entt::entity> entities = std::move(get_item_entities_sorted_by_id(registry));

	if (entities.size() && ImGui::BeginTable("ItemTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
		ImGui::TableSetupColumn("ID");
		ImGui::TableSetupColumn("Name");

		for (auto entity : entities) {
			auto& item = registry.get<Game::Component::Item>(entity);
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			ImGui::Text("%d", item.id);
			ImGui::TableNextColumn();

			char name[256];
			snprintf(name, 256, "%s", item.name.c_str());
			char label[256];
			snprintf(label, 256, "##name:%d", item.id);
			if (ImGui::InputText(label, name, 256)) {
				item.name = name;
			}
		}

		ImGui::EndTable();
	}

	ImGui::End();
}


void ItemsEditor::save_to_file(entt::registry& registry) {
	std::filesystem::path file_path = data_dir() / "items.bin";
	std::ofstream os(file_path, std::ios::binary);
	bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
	std::vector<entt::entity> entities = std::move(get_item_entities_sorted_by_id(registry));

	uint64_t count = entities.size();
	ser.value8b(count);
	for (auto entity : entities) {
		auto& item = registry.get<Game::Component::Item>(entity);

		ser.object(item);
	}

	ser.adapter().flush();
}

void ItemsEditor::load_from_file(entt::registry& registry) {
	std::filesystem::path file_path = data_dir() / "items.bin";
	std::ifstream is(file_path, std::ios::binary);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	uint64_t count = 0;
	des.value8b(count);
	for (uint64_t i = 0; i < count; ++i) {
		Game::Component::Item item;
		des.object(item);
		registry.emplace<Game::Component::Item>(registry.create(), item);
	}
}

std::vector<entt::entity> ItemsEditor::get_item_entities_sorted_by_id(entt::registry& registry) {
	auto items = registry.view<Game::Component::Item>();
	std::vector<entt::entity> entities;

	for (auto entity : items) {
		entities.push_back(entity);
	}

	std::sort(entities.begin(), entities.end(), [&registry](entt::entity a, entt::entity b) {
		auto& item_a = registry.get<Game::Component::Item>(a);
		auto& item_b = registry.get<Game::Component::Item>(b);
		return item_a.id < item_b.id;
		});

	return entities;
}

}
}
}
