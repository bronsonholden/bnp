#include <bnp/ui/file_browser.h>

#include <imgui.h>

namespace bnp {
	FileBrowser::FileBrowser(const std::string& start_path)
		: current_dir(std::filesystem::absolute(start_path)) {}

	void FileBrowser::render() {
		if (!is_open) return;

		ImGui::Begin("Select Directory", &is_open);

		if (ImGui::Button("Up") && current_dir.has_parent_path()) {
			current_dir = current_dir.parent_path();
		}

		ImGui::Text("Current Directory:\n%s", current_dir.string().c_str());
		ImGui::Separator();

		for (const auto& entry : std::filesystem::directory_iterator(current_dir)) {
			if (entry.is_directory()) {
				const std::string& folder_name = entry.path().filename().string();
				if (ImGui::Selectable(folder_name.c_str(), false)) {
					current_dir /= folder_name;
				}
			}
		}

		ImGui::Separator();
		if (ImGui::Button("Select This Folder")) {
			selected_dir = current_dir;
			is_open = false;
		}

		ImGui::End();
	}

	std::string FileBrowser::get_selected_path() const {
		return selected_dir.string();
	}

	bool FileBrowser::has_selection() const {
		return !selected_dir.empty();
	}
}
