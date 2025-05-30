#include <bnp/factories/mesh_factory.h>

namespace bnp {

void MeshFactory::create_circle(std::vector<Vertex>& out_vertices, std::vector<unsigned int>& out_indices) {
	const int num_vertices = 64;

	// Calculate the angle between consecutive vertices
	float step = 2.0f * 3.14159f / num_vertices;

	// Create vertices for the circle (in polar coordinates, converted to Cartesian)
	for (int i = 0; i < num_vertices; ++i) {
		float angle = i * step;
		float x = 0.5f * cos(angle);
		float y = 0.5f * sin(angle);

		// Add the vertex to the list
		out_vertices.push_back({
			{ x, y, 0 },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f } // no need for texcoords
			});
	}

	// Create indices for the lines (connecting consecutive vertices)
	for (int i = 0; i < num_vertices; ++i) {
		int next_idx = (i + 1) % num_vertices; // Wrap around to create a closed circle
		out_indices.push_back(i);
		out_indices.push_back(next_idx);
	}
}

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

void MeshFactory::create_box(std::vector<Vertex>& out_vertices, std::vector<uint32_t>& out_indices) {
	out_vertices = {
		{{ -0.5f, -0.5f, 0}, { 0, 0, 1 }, { 0, 0 }},
		{{ -0.5f, 0.5f, 0}, { 0, 0, 1 }, { 0, 1 }},
		{{ 0.5f, 0.5f, 0}, { 0, 0, 1 }, { 1, 1}},
		{{ 0.5f, -0.5f, 0}, { 0, 0, 1 }, { 1, 0 }}
	};

	out_indices = { 0, 2, 1, 2, 0, 3 };
}

Mesh MeshFactory::cube(float scale) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	create_cube(vertices, indices, scale);

	return create(vertices, indices);
}

Mesh MeshFactory::circle() {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	create_circle(vertices, indices);

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

	mesh.vertex_count = vertices.size();
	mesh.index_count = indices.size();

	return mesh;
}

Mesh MeshFactory::box() {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	create_box(vertices, indices);

	return create(vertices, indices);
}

Mesh MeshFactory::line() {
	std::vector<uint32_t> indices{ 0, 1 };
	std::vector<Vertex> vertices{
		{ { 0, 0, 0 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { 0, 1, 0 }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } }
	};

	return create(vertices, indices);
}

}
