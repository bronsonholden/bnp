#include <bnp/helpers/filesystem_helper.h>
#include <bnp/game/ui/chemicals_editor.h>
#include <bnp/game/components/matter.h>
#include <bnp/game/serializers/matter.hpp>

#include <imgui.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <filesystem>
#include <fstream>

namespace bnp {
namespace Game {
namespace UI {

void ChemicalsEditor::render(entt::registry& registry) {
	if (!loaded) {
		load_from_file(registry);
		loaded = true;
	}

	if (ImGui::Button("New chemical")) {
		// todo: get next ID from index?
		Game::Component::Chemical::ID next_id = 1;
		auto chemicals = registry.view<Game::Component::Chemical>();
		for (auto entity : chemicals) {
			auto& chemical = chemicals.get<Game::Component::Chemical>(entity);
			if (chemical.id >= next_id) next_id = chemical.id + 1;
		}

		registry.emplace<Game::Component::Chemical>(registry.create(), Game::Component::Chemical{
			.id = next_id,
			.name = "<New chemical>"
			});
	}

	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);

	if (ImGui::Button("Save")) {
		save_to_file(registry);
	}

	ImGui::Separator();

	if (editing_chemical_id) {
		entt::entity chemical_entity = entt::null;
		bool close = ImGui::Button("Close");

		auto view = registry.view<Game::Component::Chemical>();
		for (auto entity : view) {
			auto& chemical = view.get<Game::Component::Chemical>(entity);
			if (chemical.id == editing_chemical_id) {
				chemical_entity = entity;
				break;
			}
		}

		if (chemical_entity != entt::null) {
			auto& chemical = registry.get<Game::Component::Chemical>(chemical_entity);

			ImGui::Text(chemical.name.c_str());

			{
				char name[256];
				snprintf(name, 256, "%s", chemical.name.c_str());
				if (ImGui::InputText("Name", name, 256)) {
					chemical.name = name;
				}
			}

			{
				char formula[256];
				snprintf(formula, 256, "%s", chemical.formula.c_str());
				if (ImGui::InputText("Formula", formula, 256)) {
					chemical.formula = formula;
				}
			}

			ImGui::InputDouble("Mass (g/mol)", &chemical.molecular_mass);
			ImGui::InputDouble("Melting point (K at 1 atm)", &chemical.melting_point);
			ImGui::InputDouble("Boiling point (K at 1 atm)", &chemical.boiling_point);
			ImGui::InputDouble("Solid density (kg/m^3)", &chemical.solid_density);
			ImGui::InputDouble("Liquid density (kg/m^3)", &chemical.liquid_density);
			ImGui::InputDouble("Specific heat (gas, J/(kg K))", &chemical.specific_heat_gas);
		}
		else {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "No chemical found by ID %d", editing_chemical_id);
		}

		if (close) {
			editing_chemical_id = 0;
		}
	}
	else {
		std::vector<entt::entity> entities = std::move(get_chemical_entities_sorted_by_id(registry));

		if (entities.size() && ImGui::BeginTable("ChemicalTable", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
			ImGui::TableSetupColumn("ID");
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Formula");
			ImGui::TableSetupColumn("Actions");

			for (auto entity : entities) {
				auto& chemical = registry.get<Game::Component::Chemical>(entity);
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				ImGui::Text("%d", chemical.id);
				ImGui::TableNextColumn();

				{
					char name[256];
					snprintf(name, 256, "%s", chemical.name.c_str());
					char label[256];
					snprintf(label, 256, "##name:%d", chemical.id);
					if (ImGui::InputText(label, name, 256)) {
						chemical.name = name;
					}
					ImGui::TableNextColumn();
				}

				{
					char formula[256];
					char label[256];
					snprintf(label, 256, "##formula:%d", chemical.id);
					snprintf(formula, 256, "%s", chemical.formula.c_str());
					if (ImGui::InputText(label, formula, 256)) {
						chemical.formula = formula;
					}
					ImGui::TableNextColumn();
				}

				{
					char label[256];
					snprintf(label, 256, "Edit##id:%d", chemical.id);
					if (ImGui::Button(label)) {
						editing_chemical_id = chemical.id;
					}
				}
			}

			ImGui::EndTable();
		}
	}
}

void ChemicalsEditor::save_to_file(entt::registry& registry) {
	std::filesystem::path file_path = data_dir() / "chemicals.bin";
	std::ofstream os(file_path, std::ios::binary);
	bitsery::Serializer<bitsery::OutputStreamAdapter> ser{ os };
	std::vector<entt::entity> entities = std::move(get_chemical_entities_sorted_by_id(registry));

	uint64_t count = entities.size();
	ser.value8b(count);
	for (auto entity : entities) {
		auto& chemical = registry.get<Game::Component::Chemical>(entity);

		ser.object(chemical);
	}

	ser.adapter().flush();
}

void ChemicalsEditor::load_from_file(entt::registry& registry) {
	std::filesystem::path file_path = data_dir() / "chemicals.bin";
	std::ifstream is(file_path, std::ios::binary);
	bitsery::Deserializer<bitsery::InputStreamAdapter> des{ is };

	uint64_t count = 0;
	des.value8b(count);
	for (uint64_t i = 0; i < count; ++i) {
		Game::Component::Chemical chemical;
		des.object(chemical);
		registry.emplace<Game::Component::Chemical>(registry.create(), chemical);
	}
}

std::vector<entt::entity> ChemicalsEditor::get_chemical_entities_sorted_by_id(entt::registry& registry) {
	auto chemicals = registry.view<Game::Component::Chemical>();
	std::vector<entt::entity> entities;

	for (auto entity : chemicals) {
		entities.push_back(entity);
	}

	std::sort(entities.begin(), entities.end(), [&registry](entt::entity a, entt::entity b) {
		auto& chemical_a = registry.get<Game::Component::Chemical>(a);
		auto& chemical_b = registry.get<Game::Component::Chemical>(b);
		return chemical_a.id < chemical_b.id;
		});

	return entities;
}

}
}
}
