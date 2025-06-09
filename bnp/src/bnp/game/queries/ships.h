#pragma once

#include <bnp/game/components/ships.h>

#include <entt/entt.hpp>

namespace bnp {
namespace Game {
namespace Queries {

entt::entity get_engine_blueprint_by_id(entt::registry& registry, Component::EngineBlueprint::ID engine_blueprint_id);

double calc_fuel_requirement(entt::registry& registry, entt::entity engine_emplacement_entity);

}
}
}
