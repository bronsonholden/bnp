#pragma once

#include <vector>

#include <bnp/components/graphics.h>

namespace bnp {

	class MeshFactory {
	public:
		MeshFactory() { }

		Mesh cube(float size);
	};

}
