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
		glm::vec2 input_direction() const;

	private:
		entt::registry& registry_;
		entt::entity entity_;
		glm::vec2 input_direction_ = glm::vec2(0.0f);
	};

} // namespace bnp
