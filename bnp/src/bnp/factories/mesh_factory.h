#pragma once

#include <vector>

#include <bnp/components/graphics.h>

namespace bnp {

	class MeshFactory {
	public:
		MeshFactory() { }

		Mesh create(const std::vector<Vertex>& out_vertices, const std::vector<uint32_t> out_indices);
		void create_circle(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
		void create_cube(std::vector<Vertex>& out_vertices, std::vector<uint32_t>& out_indices, float size = 1.0f);
		void create_box(std::vector<Vertex>& out_vertices, std::vector<uint32_t>& out_indices);
		Mesh cube(float size);
		Mesh circle();
		Mesh box();
		Mesh line();
	};

}
