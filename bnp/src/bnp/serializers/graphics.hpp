#pragma once

#include <bnp/components/graphics.h>
#include <bnp/serializers/transform.hpp>

#include <bitsery/bitsery.h>

#include <iostream>
using namespace std;

namespace bnp {

	template <typename S>
	void serialize(S& s, Instances& instances) {
		uint32_t count = instances.positions.size();

		s.value4b(count);

		if (instances.positions.size() != count) {
			instances.positions.resize(count);
			instances.rotations.resize(count);
			instances.scales.resize(count);
		}

		cout << "(de)serialize instance count: " << count << endl;

		for (auto& position : instances.positions) {
			s.object(position);
		}

		for (auto& rotation : instances.rotations) {
			s.object(rotation);
		}

		for (auto& scale : instances.scales) {
			s.object(scale);
		}

		instances.update_transforms();
	}

	template <typename S>
	void serialize(S& s, Renderable& renderable) {
		s.value1b(renderable.value);
	}

} // namespace bnp
