// Game manager is responsible for loading static data

#pragma once

#include <bnp/marshaling.hpp>

namespace bnp {
namespace Game {
namespace Manager {

// todo: this can probably be moved out of game-specific code to engine code
class GameManager {
public:
	GameManager() = default;

	void initialize(entt::registry& registry);

private:
	// Persistent entities and data that are required for the game to function.
	// Save data is sort of a "layer" on top of this, as many objects will
	// reference persistent data (e.g. ships will store a ship blueprint ID
	// where they grab their base stats before upgrades/modifications).
	void load_static_data(entt::registry& registry);

	// todo: there may be another layer of data to load here, maybe called
	// "snapshot" data which is new-game state (e.g. what ships various factions have,
	// and their emplaced modules).

	// The dynamic portion of the game state that changes as the game is played.
	// Requires static data to be loaded into the registry for the game to function.
	void create_save_data(entt::registry& registry);
	void load_save_data(entt::registry& registry);

	// Destroy all entities associated with save data, leaving only the static
	// data. Used for reloading a save, exiting to main menu, etc.
	void unload_save_data(entt::registry& registry);
};

}
}
}
