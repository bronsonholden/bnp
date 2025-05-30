#pragma once

#include <bnp/components/world.h>
#include <bnp/components/global.h>
#include <bnp/core/node.hpp>

#include <entt/entt.hpp>

namespace bnp {

class World2DManager {
public:
	World2DManager();
	~World2DManager();

	void update(entt::registry& registry, float dt);
};

}
