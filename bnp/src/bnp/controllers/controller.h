#pragma once

#include <bnp/components/controllable.h>
#include <bnp/components/physics.h>

#include <entt/entt.hpp>
#include <SDL.h>
#include <glm/glm.hpp>

namespace bnp {

	class Controller {
	public:
		Controller(entt::registry& registry, entt::entity entity);

		void process_event(const SDL_Event& event);

	private:
		entt::registry& registry;
		entt::entity entity;
	};

} // namespace bnp
