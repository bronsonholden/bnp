#include <iostream>
#include "testing.h"
#include <bnp/core/node.hpp>
#include <bnp/core/window.h>
#include <bnp/components/graphics.h>
#include <bnp/graphics/mesh_factory.h>
#include <bnp/graphics/material_factory.h>
#include <bnp/managers/resource_manager.h>

// Basic vertex shader source code
const char* vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_pos;
out vec3 normal;
out vec2 tex_coords;

void main() {
    frag_pos = vec3(model * vec4(a_pos, 1.0));
    normal = normalize(mat3(transpose(inverse(model))) * a_normal);

    tex_coords = a_tex_coords;
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

TEST_CASE("cleans up unused meshes") {
	entt::registry registry;
	bnp::ResourceManager resource_manager;
	bnp::MeshFactory mesh_factory;
	bnp::Node node(registry);
	bnp::Window window;
	bnp::Mesh mesh = mesh_factory.cube(1.0f);

	node.add_component<bnp::Mesh>(mesh);
	resource_manager.add_mesh(mesh);

	window.poll();
	window.clear();

	REQUIRE(glIsVertexArray(mesh.va_id));
	REQUIRE(glIsBuffer(mesh.vb_id));
	REQUIRE(glIsBuffer(mesh.eb_id));

	registry.destroy(node.get_entity_id());

	resource_manager.cleanup(registry);

	REQUIRE(!glIsVertexArray(mesh.va_id));
	REQUIRE(!glIsBuffer(mesh.vb_id));
	REQUIRE(!glIsBuffer(mesh.eb_id));

	window.swap();
}

TEST_CASE("cleans up unused materials") {
	entt::registry registry;
	bnp::ResourceManager resource_manager;
	bnp::MaterialFactory material_factory;
	bnp::Node node(registry);
	bnp::Window window;
	bnp::Material material = material_factory.load_material({
		{bnp::ShaderType::VertexShader, vertex_shader_source},
		{bnp::ShaderType::FragmentShader, fragment_shader_source}
		});

	node.add_component<bnp::Material>(material);
	resource_manager.add_material(material);

	window.poll();
	window.clear();

	REQUIRE(glIsProgram(material.shader_id));

	registry.destroy(node.get_entity_id());

	resource_manager.cleanup(registry);

	REQUIRE(!glIsProgram(material.shader_id));

	window.swap();
}
