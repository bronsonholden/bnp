#pragma once

#include <bitsery/bitsery.h>
#include <bnp/components/transform.h>

namespace bnp {

	template <typename S>
	void serialize(S& s, glm::vec3& v) {
		s.value4b(v.x);
		s.value4b(v.y);
		s.value4b(v.z);
	}

	template <typename S>
	void serialize(S& s, glm::quat& q) {
		s.value4b(q.x);
		s.value4b(q.y);
		s.value4b(q.z);
		s.value4b(q.w);
	}

	template <typename S>
	void serialize(S& s, bnp::Transform& t) {
		s.value4b(t.position.x);
		s.value4b(t.position.y);
		s.value4b(t.position.z);
		s.value4b(t.rotation.x);
		s.value4b(t.rotation.y);
		s.value4b(t.rotation.z);
		s.value4b(t.rotation.w);
		s.value4b(t.scale.x);
		s.value4b(t.scale.y);
		s.value4b(t.scale.z);

		t.update_world_transform();
	}

}
