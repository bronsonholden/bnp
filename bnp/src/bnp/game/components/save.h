#pragma once

#include <bnp/core/logger.hpp>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Component {

// Any entity with this component attached will be serialized to a
// save file, and will be destroyed when unloading the save for any
// reason (reloading, exiting to menu, etc.)
struct SaveData {};

// todo: can probably remove since you can't serialize a zero-size struct...
// "void" structs like this will just have a byte indicating whether it was
// emplaced on an entity or not.
template <typename S>
void serialize(S& s, SaveData& _) {
}

}
}
}
