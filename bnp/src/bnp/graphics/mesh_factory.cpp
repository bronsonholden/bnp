#include <bnp/graphics/mesh_factory.h>

namespace bnp {

	void MeshFactory::create_cube(std::vector<Vertex>& out_vertices, std::vector<uint32_t>& out_indices, float size) {
		float half = size / 2.0f;

		// Cube vertices and normals (CCW winding)
		out_vertices = {
			// Front face
			{{-half, -half,  half}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 0
			{{ half, -half,  half}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
			{{ half,  half,  half}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // 2
			{{-half,  half,  half}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 3

			// Back face
			{{-half, -half, -half}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 4
			{{ half, -half, -half}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // 5
			{{ half,  half, -half}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // 6
			{{-half,  half, -half}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 7

			// Left face
			{{-half, -half, -half}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 8
			{{-half, -half,  half}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 9
			{{-half,  half,  half}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 10
			{{-half,  half, -half}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 11

			// Right face
			{{ half, -half, -half}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 12
			{{ half, -half,  half}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // 13
			{{ half,  half,  half}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 14
			{{ half,  half, -half}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // 15

			// Top face
			{{-half,  half, -half}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 16
			{{ half,  half, -half}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // 17
			{{ half,  half,  half}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 18
			{{-half,  half,  half}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // 19

			// Bottom face
			{{-half, -half, -half}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 20
			{{ half, -half, -half}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // 21
			{{ half, -half,  half}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // 22
			{{-half, -half,  half}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}  // 23
		};

		// Index data for each face (2 triangles per face)
		out_indices = {
			// Front face (CCW)
			0, 3, 2, 2, 1, 0,

			// Back face (CCW)
			4, 7, 6, 6, 5, 4,

			// Left face (CCW)
			8, 11, 10, 10, 9, 8,

			// Right face (CCW)
			12, 15, 14, 14, 13, 12,

			// Top face (CCW)
			16, 19, 18, 18, 17, 16,

			// Bottom face (CCW)
			20, 23, 22, 22, 21, 20
		};
	}

	Mesh MeshFactory::cube(float scale) {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		create_cube(vertices, indices, scale);

		return create(vertices, indices);
	}

	Mesh MeshFactory::create(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices) {

		Mesh mesh = Mesh();

		glGenVertexArrays(1, &mesh.va_id);
		glBindVertexArray(mesh.va_id);
		glGenBuffers(1, &mesh.vb_id);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vb_id);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &mesh.eb_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.eb_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

		// Vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);

		// Vertex normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		mesh.vertex_count = 36;

		return mesh;
	}

}
