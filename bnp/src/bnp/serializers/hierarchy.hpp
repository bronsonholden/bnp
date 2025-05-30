#pragma once

#include <bitsery/bitsery.h>
#include <bnp/components/transform.h>

namespace bnp {
template <typename S>
void serialize(S& s, bnp::Position& p) {
	s.value4b(p.value.x);
	s.value4b(p.value.y);
	s.value4b(p.value.z);
}

template <typename S>
void serialize(S& s, bnp::Scale& sc) {
	s.value4b(sc.value.x);
	s.value4b(sc.value.y);
	s.value4b(sc.value.z);
}
}
