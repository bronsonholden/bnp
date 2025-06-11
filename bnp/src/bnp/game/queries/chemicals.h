#pragma once

#include <bnp/game/components/matter.h>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Queries {

entt::entity get_chemical_by_id(entt::registry& registry, Component::Chemical::ID chemical_id);

}
}
}
