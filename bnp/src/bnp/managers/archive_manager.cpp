#include <bnp/managers/archive_manager.h>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/stream.h>
#include <bitsery/traits/string.h>

namespace bnp {

	ArchiveManager::ArchiveManager(std::filesystem::path _dir) : dir(_dir) {
		dir = _dir;
	}

	ArchiveManager::~ArchiveManager() {

	}

} // namespace bnp
