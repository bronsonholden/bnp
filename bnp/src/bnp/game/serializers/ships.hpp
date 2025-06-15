#pragma once

#include <bnp/game/components/ships.h>

#include <bitsery/bitsery.h>>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

namespace bnp {
namespace Game {
namespace Component {

template <typename S>
void serialize(S& s, Game::Component::ShipSegment& segment) {
	s.value4b(segment.version);
	s.value4b(segment.ship_blueprint_id);
	s.value4b(segment.id);
	s.text1b(segment.name, 256);
	s.value4b(segment.engine_slots);
	s.value4b(segment.reactor_slots);
	s.value4b(segment.solid_storage_slots);
	s.value4b(segment.fluid_storage_slots);

	segment.version = segment.latest_version;
}

template <typename S>
void serialize(S& s, Game::Component::ShipBlueprint& blueprint) {
	s.value4b(blueprint.version);
	s.value4b(blueprint.id);
	s.text1b(blueprint.name, 256);
	s.text1b(blueprint.manufacturer, 256);
	s.value8b(blueprint.mass);

	blueprint.version = blueprint.latest_version;
}

template <typename S>
void serialize(S& s, Game::Component::EngineBlueprint& blueprint) {
	s.value4b(blueprint.version);
	s.value4b(blueprint.id);
	s.text1b(blueprint.name, 256);
	s.value8b(blueprint.mass);
	s.value8b(blueprint.effiency_factor);
	s.container4b(blueprint.propulsion_recipes, 256);

	blueprint.version = blueprint.latest_version;
}

template <typename S>
void serialize(S& s, Game::Component::FluidStorageBlueprint& blueprint) {
	s.value4b(blueprint.version);
	s.value4b(blueprint.id);
	s.text1b(blueprint.name, 256);
	s.value8b(blueprint.mass);
	s.value8b(blueprint.max_volume);
	s.value8b(blueprint.max_pressure);

	blueprint.version = blueprint.latest_version;
}

}
}
}
