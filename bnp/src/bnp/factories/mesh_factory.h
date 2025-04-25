#pragma once

#include <vector>

#include <bnp/components/graphics.h>

namespace bnp {

	class MeshFactory {
	public:
		MeshFactory() { }

		Mesh create(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices);
		void create_cube(std::vector<Vertex>& out_vertices, std::vector<uint32_t>& out_indices, float size = 1.0f);
		void create_box(std::vector<Vertex>& out_vertices, std::vector<uint32_t>& out_indices);
		Mesh cube(float size);
		Mesh box();
		Mesh line();
	};

}
