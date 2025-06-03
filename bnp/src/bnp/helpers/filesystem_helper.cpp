#include <bnp/core/logger.hpp>
#include <bnp/helpers/filesystem_helper.h>

#include <shlobj.h>

namespace bnp {

std::filesystem::path data_dir() {
	std::filesystem::path root = PROJECT_ROOT;

	return root / "bnp/data";
}

std::filesystem::path save_dir() {
	// Define the path variable to store the folder path
	PWSTR path = nullptr;

	// Get the Documents folder path using the SHGetKnownFolderPath API
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path);

	if (FAILED(hr)) {
		Log::error("Unable to find Documents folder");
		return std::filesystem::path();
	}

	// Convert the path to std::filesystem::path and free the allocated memory
	std::filesystem::path documents_path(path);
	// Free the memory allocated by SHGetKnownFolderPath
	CoTaskMemFree(path);

	return documents_path / "My Games" / "The Dark Above" / "Saves";
}

}
