#pragma once

#include <string>
#include <filesystem>

namespace bnp {

	class FileBrowser {
	public:
		FileBrowser(const std::string& start_path = PROJECT_ROOT);

		void render();

		std::string get_selected_path() const;
		bool has_selection() const;

		bool is_open = true;

	private:
		std::filesystem::path current_dir;
		std::filesystem::path selected_dir;
	};

}
