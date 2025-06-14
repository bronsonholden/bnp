#pragma once

#include <bnp/core/logger.hpp>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Component {

struct SaveData {
};

template <typename S>
void serialize(S& s, SaveData& _) {
}

}
}
}
