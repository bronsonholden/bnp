#include <bnp/core/logger.hpp>
#include <bnp/game/queries/ships.h>
#include <bnp/game/components/ships.h>

namespace bnp {
namespace Game {
namespace Queries {

entt::entity get_engine_blueprint_by_id(entt::registry& registry, Component::EngineBlueprint::ID engine_blueprint_id) {
	auto blueprints = registry.view<Component::EngineBlueprint>();

	for (auto entity : blueprints) {
		if (blueprints.get<Component::EngineBlueprint>(entity).id == engine_blueprint_id) {
			return entity;
		}
	}

	return entt::null;
}

double calc_fuel_requirement(entt::registry& registry, entt::entity ship_entity) {
	auto& engine_empl = registry.get<Component::EngineEmplacement>(ship_entity);

	entt::entity bp_ent = get_engine_blueprint_by_id(registry, engine_empl.engine_blueprint_id);
	if (bp_ent == entt::null) {
		Log::warning("No engine blueprint found (id=%d)", engine_empl.engine_blueprint_id);
		return 0;
	}

	auto& engine_bp = registry.get<Component::EngineBlueprint>(bp_ent);

	double mass = 20000; // todo: get from query
	double g = 9.3; // todo: get from query
	double energy = 55.0; // todo: get from query

	double isp_factor = 100.0 * std::sqrt(2.0 * energy) / g;

	return (mass * g) / (engine_bp.effiency_factor * isp_factor);
}

}
}
}
