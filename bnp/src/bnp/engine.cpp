#include <bnp/engine.h>

#include <bnp/core/node.hpp>
#include <bnp/components/transform.h>
#include <bnp/helpers/random_float_generator.hpp>

#include <iostream>
using namespace std;

// Basic vertex shader source code
const char* vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Instanced model matrix
layout (location = 3) in mat4 model;

uniform mat4 view;
uniform mat4 projection;

out vec3 frag_pos;
out vec3 normal;
out vec2 tex_coords;

void main() {
    frag_pos = vec3(model * vec4(aPos, 1.0));
    normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    tex_coords = aTexCoords;
    gl_Position = projection * view * vec4(frag_pos, 1.0);
}
)";

// Basic fragment shader source code
const char* fragment_shader_source = R"(
#version 330 core
out vec4 frag_color;

in vec3 frag_pos;
in vec3 normal;
in vec2 tex_coords;

uniform vec3 color;

void main() {
    vec3 light_dir = normalize(vec3(2.0, 1.0, 0.3));
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * color;
    frag_color = vec4(diffuse, 1.0);
}
)";

namespace bnp {

	Engine::Engine() {
		renderer.initialize();
	}

	Engine::~Engine() {
		renderer.shutdown();
	}

	void Engine::run() {
		float accumulator = 0.0f;
		const float fixed_dt = 1.0f / 60.0f;

		time.start();

		Material material = material_factory.load_material({
			{GL_VERTEX_SHADER, vertex_shader_source},
			{GL_FRAGMENT_SHADER, fragment_shader_source}
			});

		Mesh mesh = factory.cube(1.0f);

		glm::mat4 model = glm::mat4(1.0f);

		entt::registry;
		Node cube(registry);
		cube.add_component<Position>(Position{ { 0.0f, 0.0f, 0.0f } });
		cube.add_component<Mesh>(mesh);
		cube.add_component<Material>(material);
		cube.add_component<Renderable>(true);

		Instances instances;

		const int count = 100000;

		instances.positions.reserve(count);
		instances.rotations.reserve(count);
		instances.scales.reserve(count);

		RandomFloatGenerator pos_gen(-50.0f, 50.0f);
		RandomFloatGenerator rot_gen(-1.0f, 1.0f);
		RandomFloatGenerator scale_gen(0.1, 0.8f);

		for (int i = 0; i < count; ++i) {
			instances.positions.push_back(Position{ { pos_gen.generate(), pos_gen.generate(), pos_gen.generate() } });
			instances.rotations.push_back(Rotation{ { glm::quat() } });
			instances.scales.push_back(Scale{ glm::vec3(scale_gen.generate()) });
		}

		instances.update_transforms();
		cube.add_component<Instances>(instances);

		resource_manager.add_mesh(mesh);

		Camera camera({
			glm::vec3(5.0f, 5.0f, 5.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
			});

		while (window.open) {
			window.poll();

			time.new_frame();

			float dt = time.delta_time();

			model = glm::rotate(model, 1.0f * dt, glm::vec3(0.0f, 1.0f, 0.0f));

			while (time.needs_fixed_update()) {
				fixed_update();
				time.consume_fixed_step();
			}

			// manager updates

			window.clear();

			// rendering
			//render_manager.render(registry, renderer);
			render_manager.render_instances(registry, renderer);

			window.swap();
		}

		resource_manager.cleanup(registry);
	}

	void Engine::update(float dt) {

	}

	void Engine::fixed_update() {

	}
}
