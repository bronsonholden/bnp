#pragma once

#include <bnp/components/graphics.h>
#include <bnp/serializers/transform.hpp>

#include <bitsery/bitsery.h>

#include <iostream>
using namespace std;

namespace bnp {

	template <typename S>
	void serialize(S& s, Vertex& vertex) {
		s.value4b(vertex.position[0]);
		s.value4b(vertex.position[1]);
		s.value4b(vertex.position[2]);
		s.value4b(vertex.normal[0]);
		s.value4b(vertex.normal[1]);
		s.value4b(vertex.normal[2]);
		s.value4b(vertex.tex_coords[0]);
		s.value4b(vertex.tex_coords[1]);
	}

	template <typename S>
	void serialize(S& s, Mesh& mesh) {
		s.text1b(mesh.resource_id, 256);
	}

	template <typename S>
	void serialize(S& s, MeshData& mesh_data) {
		uint32_t vertex_count = mesh_data.vertices.size();
		uint32_t index_count = mesh_data.indices.size();

		mesh_data.vertices.reserve(vertex_count);
		mesh_data.indices.reserve(index_count);

		s.value4b(vertex_count);

		if (mesh_data.vertices.size()) {
			for (auto& vertex : mesh_data.vertices) {
				s.object(vertex);
			}
		}
		else {
			while (vertex_count--) {
				Vertex vertex;
				s.object(vertex);
				mesh_data.vertices.push_back(vertex);
			}
		}

		s.value4b(index_count);
		s.container4b(mesh_data.indices, index_count);
	}

	template <typename S>
	void serialize(S& s, Material& material) {
		s.text1b(material.vertex_shader_resource_id, 256);
		s.text1b(material.fragment_shader_resource_id, 256);
	}

	template <typename S>
	void serialize(S& s, ShaderData& shader_data) {
		s.value4b(shader_data.shader_type);
		s.text1b(shader_data.source, 10485760);
	}

	template <typename S>
	void serialize(S& s, Instances& instances) {
		uint32_t count = instances.transforms.size();

		s.value4b(count);

		if (instances.transforms.size() != count) {
			instances.transforms.resize(count);
		}

		for (auto& transform : instances.transforms) {
			s.object(transform);
		}

		instances.update_transforms();
	}

	template <typename S>
	void serialize(S& s, Renderable& renderable) {
		s.value1b(renderable.value);
	}

} // namespace bnp
