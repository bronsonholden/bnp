#pragma once

#include <bnp/components/graphics.h>
#include <bnp/serializers/transform.hpp>

#include <bitsery/bitsery.h>

#include <iostream>
using namespace std;

namespace bnp {

	template <typename S>
	void serialize(S& s, Material& material) {
		s.text1b(material.vertex_shader_resource_id, material.vertex_shader_resource_id.size());
		s.text1b(material.fragment_shader_resource_id, material.fragment_shader_resource_id.size());
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
