#pragma once

#include <glm/glm.hpp>
#include <bitsery/bitsery.h>

namespace glm {

template <typename S>
void serialize(S& s, vec3& v) {
	s.value4b(v.x);
	s.value4b(v.y);
	s.value4b(v.z);
}

}
